#ifndef TYPE_CHECK_H
#define TYPE_CHECK_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"
#include "options.h"

typedef enum {
    TYPE_INTEGER,    // For numbers and arithmetic expressions
    TYPE_BOOLEAN,    // For conditions/predicates (internal use)
    TYPE_VOID,       // For procedures
    TYPE_ERROR       // For error cases
} Type;

typedef struct {
    char error_msg[256];
} TypeContext;

// Type checking function declarations
TypeContext* create_type_context(void);
void free_type_context(TypeContext* ctx);
Type check_type(TypeContext* ctx, Node* node);
const char* type_to_string(Type type);
bool run_type_checking(Node* ast, const Options *opts);

#endif // TYPE_CHECK_H

