#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"
#include "options.h"
#include "type_check.h"

typedef enum {
    SYM_CONSTANT,
    SYM_VARIABLE,
    SYM_PROCEDURE
} SymbolKind;

typedef struct Symbol {
    char* name;
    SymbolKind kind;
    Type type;          // TYPE_INTEGER for vars/consts, TYPE_VOID for procedures
    int value;          // Used for constants
    struct Symbol* next;
} Symbol;

typedef struct Scope {
    Symbol* symbols;
    struct Scope* parent;
} Scope;

typedef struct {
    Scope* current_scope;
    Scope* global_scope; // keep track of global scope across analysis phases
    char error_msg[256];
} SemanticContext;

// Semantic analysis function declarations
SemanticContext* create_semantic_context(void);
void free_semantic_context(SemanticContext* ctx);
bool analyze_semantics(SemanticContext* ctx, Node* node);
bool run_semantic_analysis(Node* ast, const Options* opt);
void dump_symbol_table(SemanticContext* ctx, FILE* out);

#endif // SEMANTIC_H
