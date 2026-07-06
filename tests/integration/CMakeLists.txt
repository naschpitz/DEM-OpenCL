# DEM Integration tests — Qt Testlib executable driving the full Simulation
# orchestration (config parsing, physics loop, HTTP control plane).
#
# Enabled with BUILD_INTEGRATION_TEST=ON. Compiles the REAL domain .cpp sources
# via ${DEM_CORE_SOURCES} (not a unity-build mirror), so it exercises the
# production code paths. The GPU cases (Phase 2 physics) need an OpenCL device
# at runtime; the Phase 1 config/state cases do not.
#
# The binary lands in ${CMAKE_BINARY_DIR} so it runs from the build/ CWD exactly
# like the DEM simulation and TestOpenCL: the Phase 2 physics cases load
# "../opencl/Simulation.cpp.cl", whose cross-kernel #includes resolve through
# OpenCLWrapper's "-I ./" (relative to CWD).

find_package(Qt6 REQUIRED COMPONENTS Test)

# DEM_CORE_SOURCES (set at the top-level scope) holds paths relative to the
# project root, which add_executable() in this subdirectory would resolve
# against tests/integration/. Rewrite them as absolute paths first.
set(DEM_CORE_SOURCES_ABS "")
foreach(_src ${DEM_CORE_SOURCES})
    list(APPEND DEM_CORE_SOURCES_ABS "${CMAKE_SOURCE_DIR}/${_src}")
endforeach()

add_executable(TestIntegration
    main.cpp
    FakeSimulationSink.cpp        FakeSimulationSink.h
    SimulationTestHarness.cpp     SimulationTestHarness.h
    TestSimulationConfig.cpp      TestSimulationConfig.h
    TestSimulationPhysics.cpp     TestSimulationPhysics.h
    TestSimulationContacts.cpp    TestSimulationContacts.h
    TestRequestMapper.cpp         TestRequestMapper.h
    ${DEM_CORE_SOURCES_ABS}
)

set_target_properties(TestIntegration PROPERTIES
    WIN32_EXECUTABLE OFF
    MACOSX_BUNDLE OFF
)

target_include_directories(TestIntegration PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/libs
)

target_link_libraries(TestIntegration PRIVATE
    Qt6::Core
    Qt6::Test
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

set_target_properties(TestIntegration PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
