#include "semantic.h"

SemanticContext* create_semantic_context() {
    SemanticContext* ctx = malloc(sizeof(SemanticContext));
    ctx->current_scope = malloc(sizeof(Scope));
    ctx->current_scope->symbols = NULL;
    ctx->current_scope->parent = NULL;
    ctx->error_msg[0] = '\0';
    return ctx;
}

static void free_symbols(Symbol* symbol) {
    while (symbol) {
        Symbol* next = symbol->next;
        free(symbol->name);
        free(symbol);
        symbol = next;
    }
}

static void free_scope(Scope* scope) {
    if (scope) {
        free_symbols(scope->symbols);
        free(scope);
    }
}

void free_semantic_context(SemanticContext* ctx) {
    if (!ctx) return;
    
    Scope* current = ctx->current_scope;
    while (current) {
        Scope* parent = current->parent;
        free_scope(current);
        current = parent;
    }
    free(ctx);
}

static bool enter_scope(SemanticContext* ctx) {
    Scope* new_scope = malloc(sizeof(Scope));
    if (!new_scope) return false;
    
    new_scope->symbols = NULL;
    new_scope->parent = ctx->current_scope;
    ctx->current_scope = new_scope;
    return true;
}

static bool leave_scope(SemanticContext* ctx) {
    if (!ctx->current_scope->parent) return false;
    
    Scope* old_scope = ctx->current_scope;
    ctx->current_scope = old_scope->parent;
    free_scope(old_scope);
    return true;
}

static Symbol* lookup_symbol_current_scope(SemanticContext* ctx, const char* name) {
    for (Symbol* s = ctx->current_scope->symbols; s; s = s->next) {
        if (strcmp(s->name, name) == 0) return s;
    }
    return NULL;
}

static Symbol* lookup_symbol(SemanticContext* ctx, const char* name) {
    for (Scope* scope = ctx->current_scope; scope; scope = scope->parent) {
        for (Symbol* s = scope->symbols; s; s = s->next) {
            if (strcmp(s->name, name) == 0) return s;
        }
    }
    return NULL;
}

static bool declare_symbol(SemanticContext* ctx, const char* name, SymbolKind kind, Type type, int value) {
    if (lookup_symbol_current_scope(ctx, name)) {
        snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                "Symbol '%s' already declared in current scope", name);
        return false;
    }
    
    Symbol* symbol = malloc(sizeof(Symbol));
    if (!symbol) return false;
    
    symbol->name = strdup(name);
    symbol->kind = kind;
    symbol->type = type;
    symbol->value = value;
    symbol->next = ctx->current_scope->symbols;
    ctx->current_scope->symbols = symbol;
    return true;
}

static bool analyze_block(SemanticContext* ctx, Node* node) {
    if (!enter_scope(ctx)) return false;
    
    // Analyze constant declarations
    for (Node* const_decl = node->left; const_decl; const_decl = const_decl->next) {
        if (const_decl->type != NODE_CONST_DECL) continue;
        
        if (!declare_symbol(ctx, const_decl->left->name, SYM_CONSTANT, 
                          TYPE_INTEGER, const_decl->right->value)) {
            return false;
        }
    }
    
    // Analyze variable declarations
    for (Node* var_decl = node->right; var_decl && var_decl->type == NODE_VAR_DECL;
         var_decl = var_decl->next) {
        if (!declare_symbol(ctx, var_decl->left->name, SYM_VARIABLE, 
                          TYPE_INTEGER, 0)) {
            return false;
        }
    }
    
    // Analyze procedure declarations
    for (Node* proc = node->right; proc; proc = proc->next) {
        if (proc->type != NODE_PROC) continue;
        
        if (!declare_symbol(ctx, proc->left->name, SYM_PROCEDURE, 
                          TYPE_VOID, 0)) {
            return false;
        }
        
        if (!analyze_block(ctx, proc->right)) {
            return false;
        }
    }
    
    // Analyze statements
    Node* stmt = node->right;
    while (stmt && stmt->type != NODE_COMPOUND && 
           stmt->type != NODE_IF && stmt->type != NODE_WHILE) {
        stmt = stmt->next;
    }
    
    if (stmt) {
        if (!analyze_semantics(ctx, stmt)) {
            return false;
        }
    }
    
    leave_scope(ctx);
    return true;
}

bool analyze_semantics(SemanticContext* ctx, Node* node) {
    if (!node) return true;
    
    switch (node->type) {
        case NODE_PROGRAM:
            return analyze_block(ctx, node->left);
            
        case NODE_ASSIGN: {
            Symbol* sym = lookup_symbol(ctx, node->left->name);
            if (!sym) {
                snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                        "Undefined identifier '%s'", node->left->name);
                return false;
            }
            if (sym->kind == SYM_CONSTANT) {
                snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                        "Cannot assign to constant '%s'", node->left->name);
                return false;
            }
            if (sym->kind == SYM_PROCEDURE) {
                snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                        "Cannot assign to procedure '%s'", node->left->name);
                return false;
            }
            return true;
        }
            
        case NODE_CALL: {
            Symbol* sym = lookup_symbol(ctx, node->left->name);
            if (!sym) {
                snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                        "Undefined procedure '%s'", node->left->name);
                return false;
            }
            if (sym->kind != SYM_PROCEDURE) {
                snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                        "'%s' is not a procedure", node->left->name);
                return false;
            }
            return true;
        }
            
        case NODE_INPUT: {
            Symbol* sym = lookup_symbol(ctx, node->left->name);
            if (!sym) {
                snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                        "Undefined identifier '%s'", node->left->name);
                return false;
            }
            if (sym->kind != SYM_VARIABLE) {
                snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                        "Cannot read into '%s' - must be a variable", node->left->name);
                return false;
            }
            return true;
        }
            
        case NODE_IF:
            return analyze_semantics(ctx, node->right);
            
        case NODE_WHILE:
            return analyze_semantics(ctx, node->right);
            
        case NODE_COMPOUND:
            for (Node* stmt = node->left; stmt; stmt = stmt->next) {
                if (!analyze_semantics(ctx, stmt)) return false;
            }
            return true;
            
        default:
            return true;
    }
}
