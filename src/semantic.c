#include "semantic.h"

// Create semantic context
SemanticContext* create_semantic_context() {
    SemanticContext* ctx = malloc(sizeof(SemanticContext));
    if (!ctx) return NULL;

    // Initialize global scope
    ctx->global_scope = malloc(sizeof(Scope));
    if (!ctx->global_scope) {
        free(ctx);
        return NULL;
    }
    ctx->global_scope->symbols = NULL;
    ctx->global_scope->parent = NULL;

    // Set current scope to global scope
    ctx->current_scope = ctx->global_scope;
    ctx->error_msg[0] = '\0';
    return ctx;
}

// Clean up symbols in a scope
static void free_symbols(Symbol* symbol) {
    while (symbol) {
        Symbol* next = symbol->next;
        free(symbol->name);  // Free the name string
        free(symbol);        // Free the symbol structure
        symbol = next;
    }
}

// Clean up a scope
static void free_scope(Scope* scope) {
    if (scope) {
        free_symbols(scope->symbols);
        free(scope);
    }
}

// Free semantic context
void free_semantic_context(SemanticContext* ctx) {
    if (!ctx) return;
    
    // Free all scopes except global scope
    Scope* current = ctx->current_scope;
    while (current && current != ctx->global_scope) {
        Scope* parent = current->parent;
        free_scope(current);
        current = parent;
    }
    
    // Free global scope last
    if (ctx->global_scope) {
        free_scope(ctx->global_scope);
    }
    
    free(ctx);
}

// Create new scope
static bool enter_scope(SemanticContext* ctx) {
    Scope* new_scope = malloc(sizeof(Scope));
    if (!new_scope) return false;
    
    new_scope->symbols = NULL;
    new_scope->parent = ctx->current_scope;
    ctx->current_scope = new_scope;
    return true;
}

// Return to parent scope
static bool leave_scope(SemanticContext* ctx) {
    if (ctx->current_scope == ctx->global_scope) return false;
    
    Scope* old_scope = ctx->current_scope;
    ctx->current_scope = old_scope->parent;
    
    // Copy symbols from old scope to current scope if they're not already there
    Symbol* sym = old_scope->symbols;
    while (sym) {
        Symbol* next = sym->next;
        sym->next = ctx->current_scope->symbols;
        ctx->current_scope->symbols = sym;
        sym = next;
    }
    
    // Free the scope structure but not its symbols
    free(old_scope);
    return true;
}

// Look up symbol in current scope only
static Symbol* lookup_symbol_current_scope(SemanticContext* ctx, const char* name) {
    if (!ctx->current_scope) return NULL;
    
    for (Symbol* s = ctx->current_scope->symbols; s; s = s->next) {
        if (strcmp(s->name, name) == 0) return s;
    }
    return NULL;
}

// Look up symbol in all accessible scopes
static Symbol* lookup_symbol(SemanticContext* ctx, const char* name) {
    for (Scope* scope = ctx->current_scope; scope; scope = scope->parent) {
        for (Symbol* s = scope->symbols; s; s = s->next) {
            if (strcmp(s->name, name) == 0) return s;
        }
    }
    return NULL;
}

// Add new symbol to current scope
static bool declare_symbol(SemanticContext* ctx, const char* name, 
                         SymbolKind kind, Type type, int value) {
    if (!ctx->current_scope || lookup_symbol_current_scope(ctx, name)) {
        snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                "Symbol '%s' already declared in current scope", name);
        return false;
    }
    
    Symbol* symbol = malloc(sizeof(Symbol));
    if (!symbol) return false;
    
    symbol->name = strdup(name);
    if (!symbol->name) {
        free(symbol);
        return false;
    }
    
    symbol->kind = kind;
    symbol->type = type;     // Set the type
    symbol->value = value;
    symbol->next = ctx->current_scope->symbols;
    ctx->current_scope->symbols = symbol;
    return true;
}

static bool analyze_block(SemanticContext* ctx, Node* node) {
    if (!node) return true;

    if (!enter_scope(ctx)) return false;
    
    // Analyze constant declarations
    for (Node* const_decl = node->left; const_decl; const_decl = const_decl->next) {
        if (const_decl->type == NODE_CONST_DECL) {
            if (!declare_symbol(ctx, const_decl->left->name, 
                              SYM_CONSTANT, TYPE_INTEGER, 
                              const_decl->right->value)) {
                return false;
            }
        }
    }
    
    // Analyze variable declarations
    Node* var_decl = node->right;
    while (var_decl && var_decl->type == NODE_VAR_DECL) {
        if (!declare_symbol(ctx, var_decl->left->name, 
                          SYM_VARIABLE, TYPE_INTEGER, 0)) {
            return false;
        }
        var_decl = var_decl->next;
    }
    
    // Analyze procedure declarations
    Node* proc = var_decl;  // Continue from where var_decl left off
    while (proc) {
        if (proc->type == NODE_PROC) {
            if (!declare_symbol(ctx, proc->left->name, 
                              SYM_PROCEDURE, TYPE_VOID, 0)) {
                return false;
            }
            if (!analyze_block(ctx, proc->right)) {
                return false;
            }
        }
        proc = proc->next;
    }
    
    // Find and analyze the main statement
    Node* stmt = node->right;
    while (stmt && stmt->type != NODE_COMPOUND && stmt->type != NODE_IF && 
           stmt->type != NODE_WHILE && stmt->type != NODE_ASSIGN &&
           stmt->type != NODE_CALL && stmt->type != NODE_INPUT) {
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

// Modified dump_symbol_table to include type information
void dump_symbol_table(SemanticContext* ctx, FILE* out) {
    fprintf(out, "\nSymbol Table:\n");
    fprintf(out, "%-20s %-10s %-10s %s\n", "Name", "Kind", "Type", "Value");
    fprintf(out, "------------------------------------------------\n");
    
    for (Scope* scope = ctx->current_scope; scope; scope = scope->parent) {
        for (Symbol* sym = scope->symbols; sym; sym = sym->next) {
            const char* kind_str = 
                sym->kind == SYM_CONSTANT ? "constant" :
                sym->kind == SYM_VARIABLE ? "variable" : "procedure";
            
            const char* type_str = 
                sym->type == TYPE_INTEGER ? "integer" :
                sym->type == TYPE_VOID ? "void" :
                sym->type == TYPE_BOOLEAN ? "boolean" : "error";
            
            fprintf(out, "%-20s %-10s %-10s ", sym->name, kind_str, type_str);
            
            if (sym->kind == SYM_CONSTANT) {
                fprintf(out, "%d", sym->value);
            } else {
                fprintf(out, "-");
            }
            fprintf(out, "\n");
        }
        fprintf(out, "------------------------------------------------\n");
    }
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
                if (!analyze_semantics(ctx, stmt)) {
                    return false;
                }
            }
            return true;
            
        default:
            return true;
    }
}

bool run_semantic_analysis(Node* ast, const Options* opts) {
    if (opts->verbose) {
        fprintf(opts->output, "Phase 2: Semantic Analysis\n");
    }
    
    SemanticContext* sem_ctx = create_semantic_context();
    if (!sem_ctx) {
        fprintf(stderr, "Error: Failed to create semantic analysis context\n");
        return false;
    }
    
    bool success = analyze_semantics(sem_ctx, ast);
    
    if (!success) {
        fprintf(stderr, "Semantic Error: %s\n", sem_ctx->error_msg);
    } else if (opts->verbose) {
        fprintf(opts->output, "Semantic analysis completed successfully\n");
    }
    
    if (opts->print_symbols) {
        //fprintf(opts->output, "\n----------------------------------------\n");
        dump_symbol_table(sem_ctx, opts->output);
    }
    
    free_semantic_context(sem_ctx);
    return success;
}
