# DEM Core — headless simulation server (Qt6 console application).

add_executable(Core
    main.cpp
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
    SolidObject.cpp      SolidObject.h
    Vector3D.cpp         Vector3D.h
    Vertex.cpp           Vertex.h
)

set_target_properties(Core PROPERTIES
    WIN32_EXECUTABLE OFF
    MACOSX_BUNDLE OFF
)

target_include_directories(Core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs
)

target_link_libraries(Core PRIVATE
    Qt6::Core
    Qt6::Network
    QtWebApp
    OpenCLWrapper
    tet
    easyzlib
    restclient-cpp
    nlohmann_json
)

# System libraries (matching the original DEM.pro: -lcurl -lOpenCL -lrestclient-cpp)
find_package(CURL REQUIRED)
find_package(OpenCL REQUIRED)
target_link_libraries(Core PRIVATE CURL::libcurl OpenCL::OpenCL)

# Output to Core/bin to match the original DESTDIR (Qt Creator / deployment expect it).
set_target_properties(Core PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
