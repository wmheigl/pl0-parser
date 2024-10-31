#include "ast.h"
#include "options.h"
#include "type_check.h"

TypeContext* create_type_context() {
    TypeContext* ctx = malloc(sizeof(TypeContext));
    ctx->error_msg[0] = '\0';
    return ctx;
}

void free_type_context(TypeContext* ctx) {
    free(ctx);
}

const char* type_to_string(Type type) {
    switch (type) {
        case TYPE_INTEGER: return "integer";
        case TYPE_BOOLEAN: return "boolean";
        case TYPE_ERROR: return "error";
        default: return "unknown";
    }
}

// Forward declarations for recursive type checking
static Type check_expression_type(TypeContext* ctx, Node* node);
static Type check_condition_type(TypeContext* ctx, Node* node);

static Type check_binary_op_type(TypeContext* ctx, Node* node) {
    Type left = check_expression_type(ctx, node->left);
    Type right = check_expression_type(ctx, node->right);
    
    if (left == TYPE_ERROR || right == TYPE_ERROR) {
        return TYPE_ERROR;
    }
    
    if (left != TYPE_INTEGER || right != TYPE_INTEGER) {
        snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                "Binary operator requires integer operands, got %s and %s",
                type_to_string(left), type_to_string(right));
        return TYPE_ERROR;
    }
    
    return TYPE_INTEGER;
}

static Type check_expression_type(TypeContext* ctx, Node* node) {
    if (!node) {
        snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                 "Node not allocated or invalid");
		return TYPE_ERROR;
	}
    
    switch (node->type) {
        case NODE_NUMBER:
            return TYPE_INTEGER;
            
        case NODE_IDENT:
            return TYPE_INTEGER;  // All variables are integers in PL/0
            
        case NODE_BINARY_OP:
            return check_binary_op_type(ctx, node);
            
        default:
            snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                    "Invalid node type in expression");
            return TYPE_ERROR;
    }
}

static Type check_condition_type(TypeContext* ctx, Node* node) {
    if (!node) {
        snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                 "Node not allocated or invalid");
		return TYPE_ERROR;
	}

    if (node->type != NODE_CONDITION) {
        snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                "Expected condition node");
        return TYPE_ERROR;
    }
    
    if (node->op == OP_ODD) {
        Type operand = check_expression_type(ctx, node->left);
        if (operand != TYPE_INTEGER) {
            snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                    "ODD operator requires integer operand, got %s",
                    type_to_string(operand));
            return TYPE_ERROR;
        }
    } else {
        Type left = check_expression_type(ctx, node->left);
        Type right = check_expression_type(ctx, node->right);
        
        if (left != TYPE_INTEGER || right != TYPE_INTEGER) {
            snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                    "Comparison requires integer operands, got %s and %s",
                    type_to_string(left), type_to_string(right));
            return TYPE_ERROR;
        }
    }
    
    return TYPE_BOOLEAN;
}

Type check_type(TypeContext* ctx, Node* node) {
    if (!node) return TYPE_VOID;
    
    switch (node->type) {
        case NODE_PROGRAM:
        case NODE_BLOCK:
        case NODE_COMPOUND:
            return TYPE_VOID;
            
        case NODE_CONST_DECL:
            if (node->right->type != NODE_NUMBER) {
                snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                        "Constant must be initialized with a number");
                return TYPE_ERROR;
            }
            return TYPE_INTEGER;
            
        case NODE_VAR_DECL:
            return TYPE_INTEGER;
            
        case NODE_PROC:
            return TYPE_VOID;
            
        case NODE_ASSIGN: {
            Type expr_type = check_expression_type(ctx, node->right);
            if (expr_type != TYPE_INTEGER) {
                snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                        "Assignment requires integer expression");
                return TYPE_ERROR;
            }
            return TYPE_VOID;
        }
            
        case NODE_IF:
        case NODE_WHILE: {
            Type cond_type = check_condition_type(ctx, node->left);
            if (cond_type != TYPE_BOOLEAN) {
                snprintf(ctx->error_msg, sizeof(ctx->error_msg),
                        "Control structure requires boolean condition");
                return TYPE_ERROR;
            }
            return TYPE_VOID;
        }
            
        case NODE_CONDITION:
            return check_condition_type(ctx, node);
            
        case NODE_INPUT:
        case NODE_OUTPUT:
        case NODE_CALL:
            return TYPE_VOID;
            
        default:
            return check_expression_type(ctx, node);
    }
}

bool run_type_checking(Node* ast, const Options *opts) {
    if (opts->verbose) {
        fprintf(opts->output, "Phase 1: Type Checking\n");
    }
    
    TypeContext* type_ctx = create_type_context();
    if (!type_ctx) {
        fprintf(stderr, "Error: Failed to create type checking context\n");
        return false;
    }
    
    Type program_type = check_type(type_ctx, ast);
    bool success = (program_type != TYPE_ERROR);
    
    if (!success) {
        fprintf(stderr, "Type Error: %s\n", type_ctx->error_msg);
    } else if (opts->verbose) {
        fprintf(opts->output, "Type checking completed successfully\n");
    }
    
    free_type_context(type_ctx);
    return success;
}


