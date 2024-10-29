#include <stdio.h>
#include "ast.h"

extern FILE* yyin;
extern int yyparse(void);
extern int yylineno;
extern Node* ast_root;

int main(int argc, char** argv) {
    if (argc > 1) {
        FILE* file = fopen(argv[1], "r");
        if (!file) {
            perror(argv[1]);
            return 1;
        }
        yyin = file;
    }

    yylineno = 1;
    int result = yyparse();
    printf("Parsing completed: %s\n", result == 0 ? "OK" : "ERROR");
    print_ast(ast_root, 0);
    fclose(yyin);

    return 0;
}
