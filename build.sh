#!/bin/bash
#
# Build DEM-OpenCL.
#
# Usage:
#   ./build.sh               # development (Debug) into build-dev — default
#   ./build.sh --release     # release (Release) into build
#   ./build.sh --development
#   ./build.sh --shared      # use the shared Qt6 kit (default: static)
#   flags may be combined, e.g. ./build.sh --release --shared
#
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

MODE="development"
KIT="static"
for arg in "$@"; do
  case "$arg" in
    --development) MODE="development" ;;
    --release)     MODE="release" ;;
    --static)      KIT="static" ;;
    --shared)      KIT="shared" ;;
    -h|--help)
      echo "usage: $0 [--development|--release] [--static|--shared]"
      echo "  --development  (default) Debug into build-dev"
      echo "  --release                Release into build"
      echo "  --static      (default) use the static Qt6 kit (self-contained binary)"
      echo "  --shared                use the shared Qt6 kit"
      exit 0 ;;
    *) echo "usage: $0 [--development|--release] [--static|--shared] (default --development --static)" >&2; exit 1 ;;
  esac
done

if [[ "$MODE" == "development" ]]; then
  BUILD_DIR="build-dev"; BUILD_TYPE="Debug"
else
  BUILD_DIR="build";     BUILD_TYPE="Release"
fi

echo "Building DEM-OpenCL into $BUILD_DIR ($BUILD_TYPE)..."
# The Qt6 kit presets live in CMakeUserPresets.json — gitignored and machine-specific
# (they pin a custom Qt6 prefix). When present (dev machine) configure via the preset
# (-B overrides its binaryDir, -D overrides CMAKE_BUILD_TYPE, so one preset serves both
# build-dev and build). When absent (fresh clone / installer target) fall back to plain
# cmake and rely on Qt6 being on the default search path (system Qt6, e.g. as installed
# by install.sh's dependency step). CMAKE_PREFIX_PATH, if set in the environment, is
# honored by both paths; the --static/--shared kit flag only applies in preset mode.
if [ -f "$SCRIPT_DIR/CMakeUserPresets.json" ]; then
  echo "  (using $KIT Qt6 kit via CMakeUserPresets.json)"
  cmake --preset "$KIT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
else
  echo "  (no CMakeUserPresets.json — plain cmake; install system Qt6 or set CMAKE_PREFIX_PATH)"
  cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
fi && cmake --build "$BUILD_DIR" -j$(nproc)
