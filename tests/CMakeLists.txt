# Tests CMakeLists.txt

# Fetch GoogleTest (BSD License)
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test Executable
add_executable(zafra_tests
    xml/xml_document_test.cpp
    document/document_factory_test.cpp
    webservice/distribuicao_dfe_test.cpp
    integration/nfe_integration_test.cpp
)

target_link_libraries(zafra_tests
    PRIVATE
        zafra
        GTest::gtest_main
)

target_compile_definitions(zafra_tests PRIVATE TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

include(GoogleTest)
gtest_discover_tests(zafra_tests)
