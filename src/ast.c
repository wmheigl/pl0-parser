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
    if (node == NULL) return;

    // Print indentation
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    // Print node information
    switch (node->type) {
        case NODE_PROGRAM:
            printf("Program\n");
            break;
        case NODE_BLOCK:
            printf("Block\n");
            break;
        case NODE_CONST_DECL:
            printf("Const Declaration: %s = %d\n", node->left->name, node->right->value);
            break;
        case NODE_VAR_DECL:
            printf("Var Declaration: %s\n", node->left->name);
            break;
        case NODE_PROC:
            printf("Procedure Declaration: %s\n", node->left->name);
            break;
        case NODE_ASSIGN:
            printf("Assignment: %s :=\n", node->left->name);
            break;
        case NODE_CALL:
            printf("Procedure Call: %s\n", node->left->name);
            break;
        case NODE_INPUT:
            printf("Input: %s\n", node->left->name);
            break;
        case NODE_OUTPUT:
            printf("Output\n");
            break;
        case NODE_COMPOUND:
            printf("Compound Statement\n");
            break;
        case NODE_IF:
            printf("If Statement\n");
            break;
        case NODE_WHILE:
            printf("While Loop\n");
            break;
        case NODE_CONDITION:
            printf("Condition: %s\n", to_string(node->op));
            break;
        case NODE_BINARY_OP:
            printf("Binary Operation: %s\n", to_string(node->op));
            break;
        case NODE_NUMBER:
            printf("Number: %d\n", node->value);
            break;
        case NODE_IDENT:
            printf("Identifier: %s\n", node->name);
            break;
    }

    // Recursively print child nodes
    print_ast(node->left, depth + 1);
    print_ast(node->right, depth + 1);

    // Print next node in list (if any)
    print_ast(node->next, depth);
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
