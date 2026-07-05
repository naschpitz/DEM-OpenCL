# DEM Test — Qt Testlib executable.
#
# Enabled with BUILD_TEST=ON. The Test executable is a unity build: main.cpp
# #includes the domain-object .cpp sources directly (Particle, Material, Scenery,
# ...). Domain objects do NOT depend on OpenCLWrapper/QtWebApp, so the unity build
# links only Qt6 + tet + OpenCL headers + the vendored nlohmann/json.hpp.

add_executable(Test
    main.cpp
    TestMaterial.cpp           TestMaterial.h
    TestMaterialsManager.cpp   TestMaterialsManager.h
    TestNonSolidObject.cpp     TestNonSolidObject.h
    TestObjectsManager.cpp     TestObjectsManager.h
    TestParticle.cpp           TestParticle.h
    TestScenery.cpp            TestScenery.h
    TestSolidObject.cpp        TestSolidObject.h
    TestVector3D.cpp           TestVector3D.h
)

set_target_properties(Test PROPERTIES
    WIN32_EXECUTABLE OFF
    MACOSX_BUNDLE OFF
)

find_package(Qt6 REQUIRED COMPONENTS Test)

target_include_directories(Test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/libs
)

find_package(OpenCL REQUIRED)
target_link_libraries(Test PRIVATE
    Qt6::Core
    Qt6::Test
    Qt6::Network
    tet
    OpenCL::OpenCL
)

set_target_properties(Test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
