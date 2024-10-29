cmake_minimum_required(VERSION 3.10)
project(PL0_Parser C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

# Find Flex and Bison
find_package(FLEX 2.6 REQUIRED)
find_package(BISON 3.8 REQUIRED)

# Generate lexer and parser
flex_target(scanner src/scanner.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c)
bison_target(parser src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c
             DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.h
             COMPILE_FLAGS "--debug")
add_flex_bison_dependency(scanner parser)

# Create the main library
add_library(pl0_lib
    src/ast.c
    ${FLEX_scanner_OUTPUTS}
    ${BISON_parser_OUTPUTS}
)

target_include_directories(pl0_lib PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Create the main executable
add_executable(pl0_parser src/main.c)
target_link_libraries(pl0_parser pl0_lib)

# Google Test
find_package(GTest REQUIRED)

# Enable testing
enable_testing()

# Add the tests subdirectory
add_subdirectory(tests)
