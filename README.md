# DEM-OpenCL

A **Discrete Element Method (DEM)** simulation server that runs the physics
time-stepping loop on the GPU via **OpenCL**, and exposes a small **HTTP API**
for start / pause / stop control. It is a headless console application built
with **C++17, Qt6, and OpenCL**, with HTTP served through the QtWebApp library.

DEM runs as a worker: an external client POSTs a simulation definition, the
server spawns the time-stepping loop on an OpenCL device, and streams frames,
requests, and logs back to the client. Multiple simulations can run
concurrently, scheduled across available GPUs.

## Highlights

- **GPU-accelerated physics** through OpenCL kernels (particles, contacts,
  faces, materials), with multi-device scheduling.
- **HTTP control plane** — start, pause, and stop simulations with a simple
  JSON API.
- **Streaming output** — simulation frames, log lines, and follow-up requests
  are pushed back to the caller (compressed with zlib / pigz where applicable).
- **Headless and service-friendly** — ships as a single `DEM` binary plus the
  `opencl/` kernel directory, and optionally registers a `systemd` unit.

## Architecture

| Layer | Responsibility |
|-------|----------------|
| `main.cpp` | Entry point; loads `DEM.ini`, starts the QtWebApp HTTP listener. |
| `RequestMapper` | Routes HTTP requests to simulation control. |
| `Simulation` | Owns the OpenCL device and drives the time-stepped physics loop. |
| `Scenery` | The simulated scene: objects, materials, particles. |
| `Particle` / `Vertex` / `Vector3D` | Core geometric/physical entities. Each maps to a flat C struct (`ParticleCL`, `VertexCL`, ...) used as OpenCL device memory. |
| `MaterialsManager` / `Material` | Material definitions and contact parameters. |
| `ObjectsManager` / `SolidObject` / `NonSolidObject` / `Face` | Scene geometry and collision boundaries. |
| `Sender` / `FrameSender` / `RequestSender` / `LogSender` | Push frames, requests, and logs back out to the caller. |
| `Deflater` / `Pigz` / `EasyZLib` | Compress simulation output. |

Two OpenCL layers coexist: **OpenCLWrapper** (`extern/OpenCLWrapper`, a git
submodule) for device/context/kernel management, and the raw OpenCL C++ bindings
for the `cl_*` types and `*CL` device-memory structs.

## Prerequisites

- C++17 compiler (GCC)
- CMake >= 3.21
- Qt6 (Core, Network) — a static or shared kit
- OpenCL ICD + headers (`libopencl` / `opencl-headers`) and a GPU driver
- libcurl
- Git (with submodule support)

## Build

`build.sh` is the supported build entry point. It mirrors a dev/release split:

```bash
./build.sh                  # Debug  -> build-dev/   (default)
./build.sh --release        # Release -> build/
./build.sh --release --shared
```

`--static` (default) / `--shared` selects the Qt6 kit via the
`static` / `shared` presets in `CMakeUserPresets.json`. That file is
**gitignored** (it pins machine-specific Qt6 paths), so on this machine it must
exist; on a fresh clone without it, `build.sh` falls back to plain `cmake`
against a system Qt6.

The executable lands at `build/DEM` (or `build-dev/DEM`). Equivalent low-level
invocation:

```bash
cmake --preset static -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

After a fresh clone, initialize the submodule:

```bash
git submodule update --init --recursive
```

## Install

`install.sh` is a full installer (modeled on the NN-Server installer). It
clones the repo, builds it, stages the binary and kernels into a self-contained
layout, writes `DEM.ini`, and optionally registers a `systemd` service:

```bash
./install.sh                            # interactive: install path + port, with systemd
./install.sh /opt/dem-opencl            # explicit install path
./install.sh --no-systemd               # skip the systemd service entirely
```

The staged layout preserves the runtime requirement that `opencl/` sits one
level above the binary's working directory:

```
<install-path>/
  bin/
    DEM                 # the executable
    DEM.ini             # runtime config (appName-derived name)
  opencl/
    *.cl                # OpenCL kernels (runtime resources, loaded at startup)
```

The `dem-opencl` service runs `bin/DEM` from `bin/` as its working directory,
so the `../opencl/` kernels and the sibling `DEM.ini` both resolve correctly.
Run `./install.sh` again to update an existing install (rebuild + restage).

## Configuration

`DEM.ini` is discovered via `QCoreApplication::applicationDirPath()` (next to
the executable, **not** the CWD), and its filename is derived from the
application name — the build target is `DEM`, so the file must be `DEM.ini`.

```ini
[listener]
;host=192.168.0.100
port=8081
minThreads=4
maxThreads=100
readTimeout=60000
maxRequestSize=500000
maxMultiPartSize=10000000

[interface]
remote=true
```

The binary takes no command-line arguments; it auto-discovers `DEM.ini`.

## HTTP API

All endpoints accept `POST` with a JSON body and return `204` on success.

| Method & path | Body | Action |
|---------------|------|--------|
| `POST /simulations/start` | `{ "_id": "...", "instance": "...", ...simulation definition }` | Start (or replace) a simulation thread. |
| `POST /simulations/pause` | `{ "_id": "..." }` | Pause a running simulation. |
| `POST /simulations/stop`  | `{ "_id": "..." }` | Stop and tear down a simulation. |

`_id` identifies the simulation; `instance` is an opaque version token — if it
differs from the currently running instance, the old simulation is stopped and
replaced. Results are streamed back to the requesting peer's address.

```bash
curl -X POST http://localhost:8081/simulations/start \
  -H 'Content-Type: application/json' \
  -d '{"_id":"sim-1","instance":"v1", ... }'
```

Error responses: `402` (missing/invalid field), `412` (precondition not met,
e.g. pausing a non-running simulation).

## Project layout

```
DEM-OpenCL/
  CMakeLists.txt        Top-level build (single DEM executable target)
  CMakeUserPresets.json Qt6 kit paths (gitignored, machine-specific)
  *.cpp / *.h           Simulation sources (flat in repo root)
  opencl/               OpenCL kernels — RUNTIME RESOURCES, not build inputs
  DEM.ini               Runtime config (appName-derived)
  libs/                 Vendored static libs (QtWebApp, tet, easyzlib, restclient-cpp, nlohmann)
  extern/OpenCLWrapper  OpenCL abstraction (git submodule)
  build.sh              Build wrapper (--development / --release, --static / --shared)
  install.sh            Full installer (clone, build, stage, DEM.ini, optional systemd)
```

See `AGENTS.md` for the detailed contributor guide (build internals, code style,
OpenCL conventions, gotchas).

## License

Released under the Apache License 2.0 with the Commons Clause restriction — see
[LICENSE.md](LICENSE.md) for details.
