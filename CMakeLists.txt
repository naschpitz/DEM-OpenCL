cmake_minimum_required(VERSION 3.21)

project(DEM LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The Qt6 kit (shared vs static) is selected by the configure preset, not here.
# See CMakeUserPresets.json (git-ignored) for the 'static' and 'shared' presets.
find_package(Qt6 REQUIRED COMPONENTS Core Network)

# Allow disabling the (legacy, currently broken) Test target.
# Test/main.cpp uses a unity build that #includes Core .cpp files, two of which
# (OpenCL.cpp, OpenCLCU.cpp) no longer exist — so Test does not currently compile.
option(BUILD_TEST "Build the legacy Test executable (currently broken — see Test/main.cpp)" OFF)

message(STATUS "DEM-OpenCL")
message(STATUS "  Qt6:            ${Qt6_DIR}")
message(STATUS "  BUILD_TEST:     ${BUILD_TEST}")

add_subdirectory(libs)
add_subdirectory(Core)

if(BUILD_TEST)
    add_subdirectory(Test)
endif()
