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
find_package(CURL REQUIRED)
find_package(OpenCL REQUIRED)

# Allow disabling the (legacy, currently broken) Test target.
# tests/main.cpp uses a unity build that #includes sources, two of which
# (OpenCL.cpp, OpenCLCU.cpp) no longer exist — so Test does not currently compile.
option(BUILD_TEST "Build the legacy Test executable (currently broken — see tests/main.cpp)" OFF)

# OpenCL kernel test harness: runs the real .cl device functions on an actual
# OpenCL device. Separate flag because it needs a device present at runtime.
option(BUILD_OPENCL_TEST "Build the OpenCL kernel test executable (requires an OpenCL device at runtime)" OFF)

# Integration tests: drive the full Simulation orchestration (config parsing,
# physics loop on a real GPU, HTTP control plane). Separate flag because the
# GPU tests need an OpenCL device at runtime, like BUILD_OPENCL_TEST.
option(BUILD_INTEGRATION_TEST "Build the integration test executable (Simulation lifecycle + physics loop; GPU cases need an OpenCL device at runtime)" OFF)

# OpenCLWrapper (git submodule, built from source).
if(NOT TARGET OpenCLWrapper)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/OpenCLWrapper ${CMAKE_CURRENT_BINARY_DIR}/OpenCLWrapper)
endif()

add_subdirectory(libs)

message(STATUS "DEM-OpenCL")
message(STATUS "  Qt6:        ${Qt6_DIR}")
message(STATUS "  BUILD_TEST:       ${BUILD_TEST}")
message(STATUS "  BUILD_OPENCL_TEST: ${BUILD_OPENCL_TEST}")
message(STATUS "  BUILD_INTEGRATION_TEST: ${BUILD_INTEGRATION_TEST}")

# --- DEM Core sources (shared by the DEM executable and the integration tests).
# Kept in a variable so the integration test target compiles the real .cpp
# sources (Simulation.cpp, Scenery.cpp, ...) instead of a unity-build mirror.
set(DEM_CORE_SOURCES
    Common.cpp           Common.h
    Deflater.cpp         Deflater.h
    EasyZLib.cpp         EasyZLib.h
    Error.cpp            Error.h
    Face.cpp             Face.h
    FrameSender.cpp      FrameSender.h
    LogSender.cpp        LogSender.h
    Material.cpp         Material.h
    MaterialsManager.cpp MaterialsManager.h
    Neighborhood.h
    NonSolidObject.cpp   NonSolidObject.h
    ObjectsManager.cpp   ObjectsManager.h
    Particle.cpp         Particle.h
    Pigz.cpp             Pigz.h
    RequestMapper.cpp    RequestMapper.h
    RequestSender.cpp    RequestSender.h
    Scenery.cpp          Scenery.h
    Sender.cpp           Sender.h
    Simulation.cpp       Simulation.h
    SimulationSink.h
    SolidObject.cpp      SolidObject.h
    Vector3D.cpp         Vector3D.h
    Vertex.cpp           Vertex.h
)

# --- DEM Core — headless simulation server (Qt6 console application) -----------
add_executable(DEM
    main.cpp
    ${DEM_CORE_SOURCES}
)

set_target_properties(DEM PROPERTIES
    WIN32_EXECUTABLE OFF
    MACOSX_BUNDLE OFF
)

target_include_directories(DEM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/libs
)

target_link_libraries(DEM PRIVATE
    Qt6::Core
    Qt6::Network
    QtWebApp
    OpenCLWrapper
    tet
    easyzlib
    restclient-cpp
    nlohmann_json
    CURL::libcurl
    OpenCL::OpenCL
)

if(BUILD_TEST)
    add_subdirectory(tests)
endif()

if(BUILD_OPENCL_TEST)
    add_subdirectory(tests/opencl)
endif()

if(BUILD_INTEGRATION_TEST)
    add_subdirectory(tests/integration)
endif()
