cmake_minimum_required(VERSION 3.14)
project(zafra VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include FetchContent to download dependencies
include(FetchContent)

add_subdirectory(extern/libxml2)
add_subdirectory(extern/tinyxml2)

# Main Library
add_library(zafra
    src/xml/xml_document.cpp
    src/document/document_factory.cpp
    src/webservice/distribuicao_dfe.cpp
    src/document/documents/nfe_document.cpp
)

target_include_directories(zafra PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(zafra PUBLIC LibXml2 tinyxml2)

# Configure Tests
enable_testing()
add_subdirectory(tests)

include(GNUInstallDirs)

target_include_directories(zafra PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS zafra LibXml2 tinyxml2
    EXPORT zafraConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}   # Static libraries (.a, .lib)
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}   # Shared libraries (.so)
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}   # DLLs on Windows (.dll)
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT zafraConfig
    FILE zafraConfig.cmake
    NAMESPACE zafra::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zafra
)