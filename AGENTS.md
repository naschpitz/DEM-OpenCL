# DEM-OpenCL — Discrete Element Method Simulation

C++17 + Qt6 + OpenCL discrete element method (DEM) simulation. Headless console application:
runs the physics loop on the GPU via OpenCL and exposes an HTTP control/status API through QtWebApp.

## Layout

```
DEM-OpenCL/
  CMakeLists.txt        Top-level build (single DEM executable target)
  CMakeUserPresets.json Qt6 kit paths (gitignored, machine-specific)
  *.cpp / *.h           Simulation sources (flat in repo root, like ANN)
  opencl/               OpenCL kernels (`.cl` / `.cpp.cl` / `.h.cl`) — RUNTIME RESOURCES, not build inputs
  DEM.ini               Runtime config (appName-derived; see Gotchas)
  libs/                 Vendored static libs (QtWebApp, tet, easyzlib, restclient-cpp, nlohmann)
  extern/OpenCLWrapper  OpenCL abstraction (git submodule)
  tests/                Legacy unity-build test (currently broken; BUILD_TEST=ON)
  depends/              External runtime deps
  build.sh              Build wrapper: --development (build-dev, Debug) / --release (build, Release), --static/--shared
  install.sh            Full installer: clone, build, stage binary+kernels, DEM.ini, optional systemd
```

## Build

```bash
./build.sh                  # development (Debug) into build-dev — default
./build.sh --release        # release (Release) into build
./build.sh --release --shared
```

`build.sh` configures via the `static`/`shared` preset from `CMakeUserPresets.json`
(machine-specific Qt6 kit prefix), then overrides the build dir (`build-dev`/`build`)
and `CMAKE_BUILD_TYPE` so one preset serves both modes. Equivalent low-level call:

```bash
cmake --preset static -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)   # builds the DEM executable -> ./build/DEM
```

Two presets in `CMakeUserPresets.json` select the Qt6 kit:
- `static`  — self-contained binary against a statically-linked Qt6 (`Qt-6.8.2-static`)
- `shared`  — links a shared Qt6 (`Qt-6.8.2-shared`)

The executable lands in the build dir (`build/DEM` / `build-dev/DEM`). There is no
separate `bin/` output directory.

### Prerequisites
- C++17 compiler (GCC)
- CMake >= 3.21
- Qt6 (Core, Network) — static or shared kit
- OpenCL ICD + headers (`libopencl` / `opencl-headers`) + a GPU driver
- libcurl
- After a fresh clone: `git submodule update --init --recursive` (pulls `extern/OpenCLWrapper`)

## Tests

**Working tests are fundamental to this project.** The `tests/` directory holds a Qt Testlib
unit-test suite (`Test` target) that covers the domain objects — `Vector3D`, `Particle`,
`Material`, `MaterialsManager`, `NonSolidObject`, `SolidObject`, `Scenery`, `ObjectsManager`.
Every change to a domain object or its JSON contract MUST keep this suite green; if you add or
change behavior, add or update the corresponding test. Do not leave the suite broken.

The `Test` target is **OFF by default** (`BUILD_TEST` CMake option). It uses a unity build:
`tests/main.cpp` `#include`s the `.cpp` sources directly (no linking of domain objects), so it
compiles fast but only covers code that has no OpenCL dependency. Fixtures live beside the tests
as `*.json` and are loaded with `std::ifstream` + `nlohmann::json::parse` (the domain constructors
take `nlohmann::json`, NOT `QJsonObject`). The binary runs from `tests/bin/`, so fixtures are
referenced as `"../Foo.json"` (resolving to `tests/`).

```bash
cmake --preset static -B build -DBUILD_TEST=ON
cmake --build build -j                  # builds the DEM target AND tests/bin/Test
( cd tests/bin && ./Test )              # run the suite (prints per-suite totals; 0 failures = green)
```

### Conventions for the tests
- **CL structs store `cl_float`.** When comparing a `*CL` field against a literal, suffix it with
  `f` (e.g. `QCOMPARE(materialCL.distanceThreshold, 4.56f)`) so QCOMPARE sees `float` vs `float`.
  Unsuffixed doubles fail QTest's fuzzy compare.
- **`Vector3D` stores `float`.** Same rule: suffix literals with `f`, and for box/bounds checks
  compare each component with `qAbs(...) < 1e-3f` rather than `QCOMPARE(Vector3D, Vector3D)` (the
  `==` operator is exact).
- **Non-solid / solid objects need `initialize()`.** Their constructors only store fields;
  particles/faces are built in `initialize()`. Call it before any `getCurrentMass()` /
  `getCurrentMomentum()` / `getCurrentKineticEnergy*()` / `getBox()` — otherwise they divide by
  zero or index an empty vector.
- **Physics sums are large.** Prefer *relative* tolerance (`qAbs(cur - exp) <= 1e-4 * qAbs(exp)`)
  over absolute for momentum/kinetic-energy over particle packings; for the internal KE (expected
  0, computed as total − external) bound the residual relative to the total.
- **Enum values are integers.** `Material::getCL().forceType` / `dragForceType` return the enum
  int (see `Material.h`), not the string. Compare against the int (e.g. `inverse_quadratic` → 3).

### OpenCL kernel tests

The `tests/opencl/` directory holds a separate Qt Testlib suite (`TestOpenCL` target) that runs the
**actual OpenCL distance kernels on a real GPU** — no C++ reference port. It covers the per-element
math the simulation relies on: `vector_getUnitary`, `edge_getClosestTo`, `particle_getClosestTo`,
`particle_isInternal`, `face_getClosestTo`, and an object-to-particle scan that mirrors the
production min-distance neighborhood loop (the path that handles non-convex geometry by per-face
independence — there is no explicit convex/concave branch in the kernels).

The `TestOpenCL` target is **OFF by default** (`BUILD_OPENCL_TEST` CMake option, independent of
`BUILD_TEST`). It links against `OpenCLWrapper` and the domain headers (Particle/Face/Vertex `.h`),
so it needs the same Qt6 + OpenCL ICD + headers as the `DEM` target.

```bash
cmake --preset static -B build -DBUILD_OPENCL_TEST=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build -j --target TestOpenCL
( cd build && ./TestOpenCL )   # MUST run from build/ so ../opencl/ resolves
```

**CWD contract (critical).** Like the sim, the binary runs from the build dir because
OpenCLWrapper resolves `#include`s via `-I ./` (relative to CWD) and the `.cl` cross-kernel includes
are written as `../opencl/<file>.cl`. Run it from anywhere else and the kernel build fails.

**Trampoline pattern.** OpenCL device functions can't be invoked directly from the host, so each
tested kernel is wrapped in a tiny `kernel void test_<func>(...)` "trampoline" under `opencl/`
(`TestsVector.cpp.cl`, `TestsEdge.cpp.cl`, `TestsParticle.cpp.cl`, `TestsFace.cpp.cl`,
`TestsObject.cpp.cl`). They `#include` the real kernel they exercise and just forward the args.
`opencl/Tests.cpp.cl` aggregates them (mirroring `Simulation.cpp.cl`'s pattern) and is the single
source file handed to `OpenCLWrapper::Core::addSourceFile()`. Add a new trampoline → append an
`#include` line to `Tests.cpp.cl`.

**Harness.** `KernelTestHarness` (`tests/opencl/KernelTestHarness.{h,cpp}`) is a thin RAII wrapper
around `OpenCLWrapper::Core`. Each `run<Algorithm>` method uses the **named buffer API**
(`writeBuffer<T>("name", vec, 0)` / `addArgument<T>("kernel", "name")` / `readBuffer<T>("name", vec, 0)`)
so device buffers are stable across calls and don't leak. Each test class owns its own
`KernelTestHarness` instance — i.e. its own `OpenCLWrapper::Core` and its own device buffer map — so
fixtures of different sizes (the 12-face cube vs the 20-face L-shape) never collide.

**Golden-value strategy.** All expected closest-points/distances are hand-derived closed-form
values, never a C++ reference implementation. Float32 GPU math varies by vendor; tolerances are
component-wise `QCOMPARE(float_f, float_f)` (qFuzzyCompare) for normal cases, with documented
loosening where needed. Literals are `f`-suffixed (same rule as the domain tests above).

**Suites (49 cases across 6 suites).** `TestSmoke` (harness sanity), `TestVector`
(`getUnitary` incl. zero→0), `TestEdge` (`getClosestTo` all regions incl. degenerate edge),
`TestParticle` (particle-particle aligned/3D/overlapping + `isInternal` outside/inside/boundary),
`TestFace` (`face_getClosestTo` inside-triangle / three edge regions / three vertices / coplanar /
radius-offset), `TestObject` (unit cube, 12 triangles), `TestNonConvexObject` (L-shape, 20 triangles,
including the concave-pocket case where the closest surface is an inner wall — the prime
silent-failure scenario for non-convex geometry).

**Known unguarded divide — documented, not fixed here.** `Face.cpp.cl:75` computes the barycentric
denominator `den = bb*cc - bc*bc` and divides by it unconditionally; a zero-area (degenerate)
triangle yields NaN/inf. `TestFace::F9` asserts the NaN to lock the current behavior. Issue #2
tracks adding a kernel guard; do NOT add the guard as part of routine test work without it.

**Object fixtures are hand-authored `FaceCL`, not JSON+`SolidObject`.** `SolidObject::loadStl()`
writes the JSON `stl` field to `temp.stl` in CWD and parses it via tetgen — a heavy new build surface
for zero kernel-math benefit (host↔device marshalling is already covered by the `Test` suite's
`TestSolidObject`). The OpenCL tests build `FaceCL` arrays directly via helpers in
`tests/opencl/TestHelpers.h` (`makeUnitCubeCL`, `makeLShapeCL`), so they exercise only the device math.

## Architecture

- **`main.cpp`** — entry point; loads `DEM.ini` via `Common::searchConfigFile()`, starts the QtWebApp HTTP listener.
- **`Simulation`** — owns the OpenCL device (via `OpenCLWrapper`) and drives the time-stepped physics loop. Writes kernels from the `.cl` resources.
- **`Scenery`** — the simulated scene: solid/non-solid objects, materials, particles.
- **`Particle` / `Vertex` / `Vector3D`** — core geometric/physical entities. Each domain object exposes a `getCL()` / `setCL()` pair mapping to a flat C struct (`ParticleCL`, `VertexCL`, …) used as OpenCL device memory.
- **`MaterialsManager` / `Material`** — material definitions and contact parameters.
- **`ObjectsManager` / `SolidObject` / `NonSolidObject` / `Face`** — scene geometry and collision boundaries.
- **HTTP layer** — `RequestMapper` (QtWebApp) routes requests; `Sender` / `FrameSender` / `RequestSender` / `LogSender` push frames, requests, and logs out.
- **Compression** — `Deflater` / `Pigz` / `EasyZLib` compress simulation output.

### OpenCL kernels (`opencl/*.cl`)
The `.cl` / `.cpp.cl` / `.h.cl` files under `opencl/` are **runtime resources**, not CMake build
inputs. At runtime the simulation runs from `build/` and loads `../opencl/Simulation.cpp.cl` (which
in turn `#include`s the other kernels). OpenCLWrapper builds with `-I ./`, so every cross-kernel
include is written as `../opencl/<file>.cl` (resolved relative to the `build/` CWD). `install.sh`
stages the same `bin/` + sibling `opencl/` layout under its install path.

### OpenCL usage
Two layers coexist:
- `OpenCLWrapper` (`extern/OpenCLWrapper`, submodule) — device/context/kernel management, included as `#include <OCLW_Core.hpp>`.
- Raw OpenCL C++ bindings — `#include <CL/opencl.hpp>` in headers that declare `cl_*` types and `*CL` structs. The `DEM` target links `OpenCL::OpenCL` directly for these.

## Code style

**Match existing style in every file — even if you'd do it differently.**

### `.h` class layout
`private:` (members) → `public:` (constructors, then methods). 4-space indent.
Domain objects declare a flat C struct (`typedef struct { ... } FooCL;`) holding `cl_*` fields for
device memory, above the C++ class.

### `.cpp`
- `this->` for all member access.
- Methods separated by a blank line; no separator banner.
- `#include "OwnHeader.h"` (own headers, quotes) ; third-party with angle brackets.

### Naming
- Types: PascalCase (`Particle`, `Vector3D`) · methods/vars: camelCase
- CL structs: `<Type>CL` (`ParticleCL`, `VertexCL`)
- No namespaces — classes live at global scope.

## Commit conventions

Conventional commits: `feat` / `fix` / `refactor` / `perf` / `test` / `docs` / `chore` / `build` / `ci`.
Imperative, lowercase subject; body explains **why**, not what. **Never** add `Co-Authored-By`.
Commit/push only when explicitly asked.

## Principles

### 1. Think before coding
Don't assume, don't hide confusion, surface tradeoffs. State assumptions explicitly; if uncertain, ask.
If multiple interpretations exist, present them — don't pick silently.

### 2. Simplicity first
Minimum code that solves the problem. No speculative features, no abstractions for single-use code.
If 200 lines could be 50, rewrite it.

### 3. Surgical changes
Touch only what you must. Don't refactor or "improve" adjacent code. Match existing style. If you
notice unrelated dead code, mention it — don't delete it. Every changed line should trace to the request.

### 4. Goal-driven execution
Define success criteria and loop until verified. "Fix the bug" → write a test that reproduces it,
then make it pass.

## OpenCLWrapper

Submodule at `extern/OpenCLWrapper` (remote: https://github.com/naschpitz/openCLWrapper.git), built
from source via `add_subdirectory`, guarded by `if(NOT TARGET OpenCLWrapper)`. To change OpenCLWrapper
itself, edit in the submodule then bump the pointer (see `submodule-sync` skill).

## Gotchas

- **Config filename is appName-derived.** `Common::searchConfigFile()` builds the name from
  `QCoreApplication::applicationName()` + `.ini`. The executable target is `DEM`, so the runtime
  config file must be `DEM.ini` (kept in the repo root, deployed to `bin/`). Renaming the target
  renames the expected ini.
- **`.cl` files are runtime resources.** They live under `opencl/`, are NOT listed as CMake sources,
  and are not compiled — they must ship one level above the binary's working directory (in dev the
  binary runs from `build/` against `../opencl/`; `install.sh` stages the same `bin/` + sibling
  `opencl/` layout on the target).
- **Prefixed includes.** `#include "nlohmann/json.hpp"` and `#include "restclient-cpp/restclient.h"`
  rely on `libs/` being on the include path (a PRIVATE include dir on the `DEM` target), not on the
  `nlohmann_json` / `restclient-cpp` INTERFACE targets' own include dirs.
- **Two OpenCL include styles.** `#include <OCLW_Core.hpp>` (unprefixed, from OpenCLWrapper) vs
  `#include <CL/opencl.hpp>` (raw bindings). Both are intentional; don't "normalize" one to the other.
