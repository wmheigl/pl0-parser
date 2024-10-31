#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "options.h"
#include "ast.h"
#include "type_check.h"
#include "semantic.h"

extern FILE* yyin;
extern int yyparse(void);
extern int yylineno;
extern Node* ast_root;

static void print_phase_separator(FILE* out) {
    fprintf(out, "\n----------------------------------------\n");
}

/* Main function */
int main(int argc, char** argv) {
    Options opts = parse_options(argc, argv);
    
    // Open input file
    yyin = fopen(opts.input_file, "r");
    if (!yyin) {
        perror(opts.input_file);
        return 1;
    }

    // Phase 0: Parsing
    if (opts.verbose) {
        print_phase_separator(opts.output);
        fprintf(opts.output, "Phase 0: Parsing\n");
    }
    
    yylineno = 1;
    int parse_result = yyparse();
    
    if (parse_result != 0) {
        fprintf(stderr, "Parse Error: Failed to parse input\n");
        fclose(yyin);
        if (opts.output != stdout) fclose(opts.output);
        return 1;
    }
    
    if (opts.verbose) {
        fprintf(opts.output, "Parsing completed successfully\n");
    }
    
    // Print AST if requested
    if (opts.print_ast) {
        print_phase_separator(opts.output);
        fprintf(opts.output, "Abstract Syntax Tree:\n");
        print_ast(ast_root, 0);
    }
    
    // Phase 1: Type Checking
    if (!opts.skip_type_check) {
        print_phase_separator(opts.output);
        if (!run_type_checking(ast_root, &opts)) {
            fclose(yyin);
            if (opts.output != stdout) fclose(opts.output);
            return 1;
        }
    }
    
    // Phase 2: Semantic Analysis
    if (!opts.skip_semantics) {
        print_phase_separator(opts.output);
        if (!run_semantic_analysis(ast_root, &opts)) {
            fclose(yyin);
            if (opts.output != stdout) fclose(opts.output);
            return 1;
        }
    }
    
    // Success
    if (opts.verbose) {
        print_phase_separator(opts.output);
        fprintf(opts.output, "All analysis phases completed successfully\n");
    }
    
    fclose(yyin);
    if (opts.output != stdout) fclose(opts.output);
    return 0;
}

