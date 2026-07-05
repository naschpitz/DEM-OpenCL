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

The `tests/` target is **OFF by default** (`BUILD_TEST` option). It is currently broken:
`tests/main.cpp` uses a unity build that `#include`s source files directly, two of which
(`OpenCL.cpp`, `OpenCLCU.cpp`) no longer exist. Enabling `BUILD_TEST=ON` will fail to compile
until that unity build is repaired.

```bash
cmake --preset static -DBUILD_TEST=ON       # will NOT compile — see above
```

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
