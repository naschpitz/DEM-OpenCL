# QtWebApp HTTP server library (replaces libs/QtWebApp/httpserver/httpserver.pri).
# Sources are compiled directly into a static library that Core links against.

add_library(QtWebApp STATIC
    httpserver/httpglobal.cpp
    httpserver/httplistener.cpp
    httpserver/httpconnectionhandler.cpp
    httpserver/httpconnectionhandlerpool.cpp
    httpserver/httprequest.cpp
    httpserver/httpresponse.cpp
    httpserver/httpcookie.cpp
    httpserver/httprequesthandler.cpp
    httpserver/httpsession.cpp
    httpserver/httpsessionstore.cpp
    httpserver/staticfilecontroller.cpp
)

target_include_directories(QtWebApp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/httpserver)
target_link_libraries(QtWebApp PUBLIC Qt6::Network)
