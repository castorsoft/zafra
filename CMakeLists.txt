cmake_minimum_required(VERSION 3.14)
project(zafra VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Place all binaries and DLLs in the same output directory so executables/tests find DLLs on Windows
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include FetchContent to download dependencies
include(FetchContent)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(ZAFRA_IS_TOP_LEVEL ON)
else()
    set(ZAFRA_IS_TOP_LEVEL OFF)
endif()
option(ZAFRA_BUILD_TESTS "Build Zafra unit and integration tests" ${ZAFRA_IS_TOP_LEVEL})

set(LIBXML2_WITH_ICONV OFF CACHE BOOL "" FORCE)
set(LIBXML2_WITH_LZMA OFF CACHE BOOL "" FORCE)
set(LIBXML2_WITH_PYTHON OFF CACHE BOOL "" FORCE)
set(LIBXML2_WITH_ZLIB OFF CACHE BOOL "" FORCE)
set(LIBXML2_WITH_TESTS OFF CACHE BOOL "" FORCE)
set(LIBXML2_WITH_PROGRAMS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    libxml2
    GIT_REPOSITORY https://gitlab.gnome.org/GNOME/libxml2.git
    GIT_TAG v2.15.4
    GIT_SHALLOW TRUE
)

set(tinyxml2_BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    tinyxml2
    GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
    GIT_TAG 11.0.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(libxml2 tinyxml2)

# Main Library
add_library(zafra
    src/xml/xml_document.cpp
    src/document/document_factory.cpp
    src/webservice/distribuicao_dfe.cpp
    src/document/documents/nfe_document.cpp
)

set_target_properties(zafra PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)


include(GNUInstallDirs)

target_include_directories(zafra PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(zafra PUBLIC LibXml2 tinyxml2)

# Configure Tests
if(ZAFRA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

install(TARGETS zafra LibXml2 tinyxml2
    EXPORT zafraConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}   # Static libraries (.a, .lib)
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}   # Shared libraries (.so)
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}   # DLLs on Windows (.dll)
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT zafraConfig
    FILE zafraConfig.cmake
    NAMESPACE zafra::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zafra
)