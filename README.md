# PL/0 Parser

A compiler front-end implementation for the PL/0 programming language, featuring
lexical analysis, syntax parsing, and abstract syntax tree (AST) generation.

## Overview

This project implements a parser for PL/0 as described in Niklaus Wirth's 1986
book *Compilerbau*, a simple programming language designed to illustrate the
most important fundamentals of programming languages and compilers. While the
author develops quite elegantly the necessary components (scanner, parser,
interpreter) from scratch, this project makes use of established tools:
Flex for lexical analysis and Bison for parsing, generating an abstract syntax
tree (AST) representation of the input program.

Type checking and semantic analysis are also implemented, code generation is not.

## Prerequisites

- CMake (version 3.10 or higher)
- Flex (version 2.6 or higher)
- Bison (version 3.8 or higher)
- C compiler supporting C11
- C++ compiler supporting C++14
- Google Test framework

## Building the Project

1. Create a build directory: ```mkdir build && cd build ```

2. Generate build files: ```cmake ..  ```

3. Build the project: ```make ```

This will create two executables:
- `pl0_parser`: The main parser executable
- `./tests/run_tests`: The test suite executable

```make help``` lists available targets.

## Project Structure

- `src/`: Source code files
  - `ast.c/h`: AST implementation
  - `semantic.c/h`: semantic analysis implementation
  - `parser.y`: Bison grammar file
  - `scanner.l`: Flex lexer file
  - `main.c`: Main program entry point
- `tests/`: Test files
  - `test-lexer.cpp`: Lexical analyzer tests
  - `test-parser.cpp`: Parser tests
  - `test-ast.cpp`: AST tests
  - `test-analysis.cpp`: semantic analysis tests
- `examples/`: Example PL/0 programs
- `pl0.ebnf`: Language grammar in EBNF notation

## Usage

To parse a PL/0 program: ```./pl0_parser input_file.pl0 ```

To run the tests: ```make test`` or ````./tests/run_tests ``` 

## Grammar

The parser supports the full PL/0 grammar, including:
- Constant declarations
- Variable declarations
- Procedure declarations
- Basic statements (assignment, call, read, write)
- Control structures (if-then, while-do)
- Expressions and conditions
- Compound statements (BEGIN...END blocks)

## Testing

The project includes comprehensive tests for:
- Lexical analysis
- Syntax parsing
- AST construction
- Complex program parsing

Run the tests to ensure everything is working correctly after making changes.

## License

This project is available under the MIT License. See the LICENSE file for
details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Run the tests
5. Submit a pull request

## Notes

- The parser performs syntax analysis only
- Error messages include line numbers for easy debugging
- The AST can be printed for visualization of the parse tree
- The project is being developed on a 2023 Mac mini (M2 chip) Sequoia 15.0.1 
