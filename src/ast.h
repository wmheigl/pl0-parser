#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// AST node types
typedef enum {
    NODE_PROGRAM,
    NODE_BLOCK,
    NODE_CONST_DECL,
    NODE_VAR_DECL,
    NODE_PROC,
    NODE_ASSIGN,
    NODE_CALL,
    NODE_INPUT,
    NODE_OUTPUT,
    NODE_COMPOUND,
    NODE_IF,
    NODE_WHILE,
    NODE_CONDITION,
    NODE_BINARY_OP,
    NODE_NUMBER,
    NODE_IDENT
} NodeType;

// AST operator types
typedef enum {
    OP_PLUS,
    OP_MINUS,
    OP_MULT,
    OP_DIV,
    OP_ODD,
    OP_EQ,
    OP_NEQ,
    OP_LT,
    OP_LTE,
    OP_GT,
    OP_GTE
} OpType;

// AST node structure
typedef struct Node {
    NodeType type;
    struct Node *left;
    struct Node *right;
    struct Node *next;  // For lists of nodes
    union {
        int value;         // For numbers
        char *name;        // For identifiers
        OpType op;         // For operators
    };
} Node;

// Function prototypes
Node* new_node(NodeType type);
Node* new_ident(const char* name);
Node* new_number(int value);
void print_ast(Node* node, int depth);

#endif // AST_H
