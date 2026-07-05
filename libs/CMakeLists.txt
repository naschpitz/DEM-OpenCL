# Third-party / vendored libraries.
#
# Three prebuilt static archives ship with the repo (no sources available), so
# they are exposed as IMPORTED STATIC targets. The include directory on each
# target is PUBLIC so consumers (Core) pick up the headers automatically.

add_subdirectory(QtWebApp)

# OpenCLWrapper is now built from source as a git submodule at extern/OpenCLWrapper
# (see root CMakeLists.txt). It is no longer a prebuilt archive under libs/.

# --- tetgen 1.5.1 (prebuilt static libtet.a) ----------------------------------
add_library(tet STATIC IMPORTED GLOBAL)
set_target_properties(tet PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/tetgen1.5.1/libtet.a
)
target_include_directories(tet INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/tetgen1.5.1)

# --- easyzlib (prebuilt static libeasyzlib.a) ---------------------------------
add_library(easyzlib STATIC IMPORTED GLOBAL)
set_target_properties(easyzlib PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/easyzlib/libeasyzlib.a
)
target_include_directories(easyzlib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/easyzlib)

# --- Header-only vendored libs ------------------------------------------------
# restclient-cpp ships only headers here; the implementation lives in the
# system-installed shared library (/usr/local/lib/librestclient-cpp.so).
# The INTERFACE target exposes both the local headers and the system library.
find_library(RestClient_LIB NAMES restclient-cpp
    PATHS /usr/local/lib /usr/lib/x86_64-linux-gnu)
add_library(restclient-cpp INTERFACE)
target_include_directories(restclient-cpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/restclient-cpp)
if(RestClient_LIB)
    target_link_libraries(restclient-cpp INTERFACE ${RestClient_LIB})
else()
    message(WARNING "librestclient-cpp not found — link will fail if RestClient:: symbols are used")
endif()

add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/nlohmann)
