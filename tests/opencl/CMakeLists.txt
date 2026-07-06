# DEM OpenCL kernel test - Qt Testlib executable running the real .cl device
# functions on an actual OpenCL device.
#
# Enabled with BUILD_OPENCL_TEST=ON (requires an OpenCL device at runtime).
# The TestOpenCL executable is placed in ${CMAKE_BINARY_DIR} so it can be run
# from the build/ CWD exactly like the DEM simulation: the kernel source is
# loaded as "../opencl/Tests.cpp.cl" and the .cl cross-kernel includes resolve
# through OpenCLWrapper's "-I ./" (relative to CWD, i.e. the build dir).

add_executable(TestOpenCL
    main.cpp
    KernelTestHarness.cpp   KernelTestHarness.h
    TestSmoke.cpp           TestSmoke.h
    TestVector.cpp          TestVector.h
    TestEdge.cpp            TestEdge.h
    TestParticle.cpp        TestParticle.h
    TestFace.cpp            TestFace.h
    TestObject.cpp          TestObject.h
    TestNonConvexObject.cpp TestNonConvexObject.h
    TestMaterial.cpp          TestMaterial.h
)

set_target_properties(TestOpenCL PROPERTIES
    WIN32_EXECUTABLE OFF
    MACOSX_BUNDLE OFF
)

find_package(Qt6 REQUIRED COMPONENTS Test)

target_include_directories(TestOpenCL PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/libs
)

target_link_libraries(TestOpenCL PRIVATE
    Qt6::Core
    Qt6::Test
    OpenCLWrapper
    OpenCL::OpenCL
)

set_target_properties(TestOpenCL PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
