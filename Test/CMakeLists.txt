# DEM Test — Qt Testlib executable.
#
# WARNING: this target is OFF by default (BUILD_TEST=ON to enable) because it is
# currently broken. Test/main.cpp uses a unity build that #includes Core .cpp
# files, two of which (OpenCL.cpp, OpenCLCU.cpp) no longer exist in Core/.
# The migration preserves the qmake structure faithfully; fixing the unity build
# is a separate task.

add_executable(Test
    main.cpp
    TestMaterial.cpp           TestMaterial.h
    TestMaterialsManager.cpp   TestMaterialsManager.h
    TestNonSolidObject.cpp     TestNonSolidObject.h
    TestObjectsManager.cpp     TestObjectsManager.h
    TestOpenCL.cpp             TestOpenCL.h
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core
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
