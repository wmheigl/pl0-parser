#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>
#include <stdio.h>

/* Command line options structure */
typedef struct {
    bool print_ast;           // -d, --debug: print AST
    bool print_symbols;       // -s, --symbols: print symbol table
    bool verbose;            // -v, --verbose: detailed output
    bool skip_type_check;    // --no-types: skip type checking
    bool skip_semantics;     // --no-semantics: skip semantic analysis
    const char* input_file;  // Input file path
    FILE* output;            // Output file (stdout or specified file)
} Options;

/* Function declarations */
void print_usage(const char* program_name);
Options parse_options(int argc, char** argv);

#endif // OPTIONS_H

