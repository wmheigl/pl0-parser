#include <stdio.h>
#include "ast.h"
#include "type_check.h"
#include "semantic.h"

extern FILE* yyin;
extern int yyparse(void);
extern int yylineno;
extern Node* ast_root;

static void print_phase_separator(void) {
    printf("\n----------------------------------------\n");
}

static bool run_type_checking(Node* ast) {
    printf("Phase 1: Type Checking\n");
    
    TypeContext* type_ctx = create_type_context();
    if (!type_ctx) {
        fprintf(stderr, "Error: Failed to create type checking context\n");
        return false;
    }
    
    Type program_type = check_type(type_ctx, ast);
    bool success = (program_type != TYPE_ERROR);
    
    if (!success) {
        fprintf(stderr, "Type Error: %s\n", type_ctx->error_msg);
    } else {
        printf("Type checking completed successfully\n");
    }
    
    free_type_context(type_ctx);
    return success;
}

static bool run_semantic_analysis(Node* ast) {
    printf("Phase 2: Semantic Analysis\n");
    
    SemanticContext* sem_ctx = create_semantic_context();
    if (!sem_ctx) {
        fprintf(stderr, "Error: Failed to create semantic analysis context\n");
        return false;
    }
    
    bool success = analyze_semantics(sem_ctx, ast);
    
    if (!success) {
        fprintf(stderr, "Semantic Error: %s\n", sem_ctx->error_msg);
    } else {
        printf("Semantic analysis completed successfully\n");
    }
    
    free_semantic_context(sem_ctx);
    return success;
}

int main(int argc, char** argv) {
    // Open input file if provided
    if (argc > 1) {
        FILE* file = fopen(argv[1], "r");
        if (!file) {
            perror(argv[1]);
            return 1;
        }
        yyin = file;
    }

    // Phase 0: Parsing
    print_phase_separator();
    printf("Phase 0: Parsing\n");
    
    yylineno = 1;
    int parse_result = yyparse();
    
    if (parse_result != 0) {
        fprintf(stderr, "Parse Error: Failed to parse input\n");
        fclose(yyin);
        return 1;
    }
    
    printf("Parsing completed successfully\n");
    
    // Print AST if debug flag is set
    if (argc > 2 && strcmp(argv[2], "-d") == 0) {
        print_phase_separator();
        printf("Abstract Syntax Tree:\n");
        print_ast(ast_root, 0);
    }
    
    // Phase 1: Type Checking
    print_phase_separator();
    bool type_check_success = run_type_checking(ast_root);
    if (!type_check_success) {
        fclose(yyin);
        return 1;
    }
    
    // Phase 2: Semantic Analysis
    print_phase_separator();
    bool semantic_analysis_success = run_semantic_analysis(ast_root);
    if (!semantic_analysis_success) {
        fclose(yyin);
        return 1;
    }
    
    // Success
    print_phase_separator();
    printf("All analysis phases completed successfully\n\n");
    
    fclose(yyin);
    return 0;
}
