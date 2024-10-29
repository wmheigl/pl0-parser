#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"
#include "type_check.h"

typedef enum {
    SYM_CONSTANT,
    SYM_VARIABLE,
    SYM_PROCEDURE
} SymbolKind;

typedef struct Symbol {
    char* name;
    SymbolKind kind;
    Type type;          // Always TYPE_INTEGER for vars/consts, TYPE_VOID for procedures
    int value;          // Used for constants
    struct Symbol* next;
} Symbol;

typedef struct Scope {
    Symbol* symbols;
    struct Scope* parent;
} Scope;

typedef struct {
    Scope* current_scope;
    char error_msg[256];
} SemanticContext;

// Semantic analysis function declarations
SemanticContext* create_semantic_context(void);
void free_semantic_context(SemanticContext* ctx);
bool analyze_semantics(SemanticContext* ctx, Node* node);

#endif // SEMANTIC_H
