# Add the test executable
add_executable(run_tests
    test-lexer.cpp
    test-parser.cpp
    test-ast.cpp
    test-analysis.cpp
)

target_link_libraries(run_tests
    PRIVATE
    pl0_lib
    GTest::GTest
    GTest::Main
)

target_include_directories(run_tests 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_BINARY_DIR}/..
#    /opt/local/include  # Add this line to include the local Google Test headers
)

# Add the test to CTest
include(GoogleTest)
gtest_discover_tests(run_tests)
