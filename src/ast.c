#include "ast.h"

// Function to create a new AST node
Node* new_node(NodeType type) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->type = type;
    node->left = node->right = node->next = NULL;
    return node;
}

// Function to create a new identifier node
Node* new_ident(const char* name) {
    Node* node = new_node(NODE_IDENT);
    node->name = strdup(name);
    return node;
}

// Function to create a new number node
Node* new_number(int value) {
    Node* node = new_node(NODE_NUMBER);
    node->value = value;
    return node;
}

// Helper function to convert OpType into a string
const char* to_string(OpType op) {
    switch (op) {
        case OP_PLUS:  return "PLUS";
        case OP_MINUS: return "MINUS";
        case OP_MULT:  return "MULT";
        case OP_DIV:   return "DIV";
        case OP_ODD:   return "ODD";
        case OP_EQ:    return "EQ";
        case OP_NEQ:   return "NEQ";
        case OP_LT:    return "LT";
        case OP_LTE:   return "LTE";
        case OP_GT:    return "GT";
        case OP_GTE:   return "GTE";
        default:       return "UNKNOWN";
    }
}

// Function to print the AST
void print_ast(Node* node, int depth) {
    fprint_ast(stdout, node, depth);
}

void fprint_ast(FILE* out, Node* node, int depth) {
    if (node == NULL) return;

    // Print indentation
    for (int i = 0; i < depth; i++) {
        fprintf(out, "  ");
    }

    // Print node information
    switch (node->type) {
        case NODE_PROGRAM:
            fprintf(out, "Program\n");
            break;
        case NODE_BLOCK:
            fprintf(out, "Block\n");
            break;
        case NODE_CONST_DECL:
            fprintf(out, "Const Declaration: %s = %d\n", node->left->name, node->right->value);
            break;
        case NODE_VAR_DECL:
            fprintf(out, "Var Declaration: %s\n", node->left->name);
            break;
        case NODE_PROC:
            fprintf(out, "Procedure Declaration: %s\n", node->left->name);
            break;
        case NODE_ASSIGN:
            fprintf(out, "Assignment: %s :=\n", node->left->name);
            break;
        case NODE_CALL:
            fprintf(out, "Procedure Call: %s\n", node->left->name);
            break;
        case NODE_INPUT:
            fprintf(out, "Input: %s\n", node->left->name);
            break;
        case NODE_OUTPUT:
            fprintf(out, "Output\n");
            break;
        case NODE_COMPOUND:
            fprintf(out, "Compound Statement\n");
            break;
        case NODE_IF:
            fprintf(out, "If Statement\n");
            break;
        case NODE_WHILE:
            fprintf(out, "While Loop\n");
            break;
        case NODE_CONDITION:
            fprintf(out, "Condition: %s\n", to_string(node->op));
            break;
        case NODE_BINARY_OP:
            fprintf(out, "Binary Operation: %s\n", to_string(node->op));
            break;
        case NODE_NUMBER:
            fprintf(out, "Number: %d\n", node->value);
            break;
        case NODE_IDENT:
            fprintf(out, "Identifier: %s\n", node->name);
            break;
    }

    // Recursively print child nodes
    fprint_ast(out, node->left, depth + 1);
    fprint_ast(out, node->right, depth + 1);

    // Print next node in list (if any)
    fprint_ast(out, node->next, depth);
}

// Helper function to reverse a linked list of nodes
Node* reverse_list(Node* head) {
    Node *prev = NULL, *current = head, *next = NULL;
    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    return prev;
}

// Helper function to find the last node in a list
Node* find_last_node(Node* head) {
    if (head == NULL) return NULL;
    while (head->next != NULL) {
        head = head->next;
    }
    return head;
}
