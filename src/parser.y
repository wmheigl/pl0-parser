/* Prologue */
%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

Node* ast_root = NULL;
extern Node* reverse_list(Node* head);
extern Node* find_last_node(Node* head);

extern FILE *yyin;
extern int yylex();
extern int yylineno;
void yyerror(const char *s);
%}

/* Bison declarations */
%define api.token.prefix {TOK_}
%define parse.error detailed

%union {
    int     value;
    char*   name;
    struct Node* node;
}

/* terminals */
%token <value>    NUM
%token <name>     IDENT

%token  CONST   VAR    PROC
%token  ASSIGN  CALL   READ   WRITE
%token  BEGIN   END    IF     THEN   WHILE   DO
%token  ODD     EQ     NEQ    LT     LTE    GT     GTE
%token  PLUS    MINUS  MULT   DIV
%token  LPAREN  RPAREN
%token  SEMICOLON  COMMA   DOT

/* non-terminals */
%type <node>  program block constants variables procedures
%type <node>  const_decl var_decl statement statement_list
%type <node>  expression condition term factor

/* Precence rules are implicit via grammar rules
%right ASSIGN
%left  LT GT EQ NEQ LTE GTE
%left  PLUS MINUS
%left  MULT DIV
*/

%%
/* Grammar rules */

program
    : block DOT
        {
            $$ = new_node(NODE_PROGRAM);
            $$->left = $1;
            ast_root = $$;
        }
    ;

block
    : constants variables procedures statement
        {
            $$ = new_node(NODE_BLOCK);
            
            // Reverse the lists before linking
            Node* const_list = reverse_list($1);
            Node* var_list = reverse_list($2);
            Node* proc_list = reverse_list($3);
            
            $$->left = const_list;
            $$->right = var_list ? var_list : (proc_list ? proc_list : $4);
            
            if (var_list) {
                Node* last_var = find_last_node(var_list);
                last_var->next = proc_list ? proc_list : $4;
            }
            
            if (proc_list) {
                Node* last_proc = find_last_node(proc_list);
                last_proc->next = $4;
            }
        }
    ;

constants
    : %empty                        { $$ = NULL; }
    | CONST const_decl SEMICOLON    { $$ = $2; }
    ;

const_decl
    : IDENT EQ NUM
        {
            $$ = new_node(NODE_CONST_DECL);
            $$->left = new_ident($1);    // identifier
            $$->right = new_number($3);   // value
        }
    | const_decl COMMA IDENT EQ NUM
        {
            $$ = new_node(NODE_CONST_DECL);
            $$->left = new_ident($3);
            $$->right = new_number($5);
            $$->next = $1;               // link to previous declarations
        }
    ;

variables
    : %empty                    { $$ = NULL; }
    | VAR var_decl SEMICOLON    { $$ = $2; }
    ;

var_decl
    : IDENT
        {
            $$ = new_node(NODE_VAR_DECL);
            $$->left = new_ident($1);
        }
    | var_decl COMMA IDENT
        {
            $$ = new_node(NODE_VAR_DECL);
            $$->left = new_ident($3);
            $$->next = $1;
        }
    ;

procedures
    : %empty                                    { $$ = NULL; }
    | procedures PROC IDENT SEMICOLON block SEMICOLON
        {
            $$ = new_node(NODE_PROC);
            $$->left = new_ident($3);
            $$->right = $5;
            $$->next = $1;
        }
    ;

statement
    : %empty                              { $$ = NULL; }
    | IDENT ASSIGN expression
        {
            $$ = new_node(NODE_ASSIGN);
            $$->left = new_ident($1);
            $$->right = $3;
        }
    | CALL IDENT
        {
            $$ = new_node(NODE_CALL);
            $$->left = new_ident($2);
        }
    | READ IDENT
        {
            $$ = new_node(NODE_INPUT);
            $$->left = new_ident($2);
        }
    | WRITE expression
        {
            $$ = new_node(NODE_OUTPUT);
            $$->left = $2;
        }
    | BEGIN statement statement_list END
        {
            $$ = new_node(NODE_COMPOUND);
            $$->left = $2;
            $$->right = $3;
        }
    | IF condition THEN statement
        {
            $$ = new_node(NODE_IF);
            $$->left = $2;
            $$->right = $4;
        }
    | WHILE condition DO statement
        {
            $$ = new_node(NODE_WHILE);
            $$->left = $2;
            $$->right = $4;
        }
    ;

statement_list
    : %empty                            { $$ = NULL; }
    | SEMICOLON statement statement_list
        {
            $$ = $2;
            $$->next = $3;
        }
    ;

condition
    : ODD expression
        {
            $$ = new_node(NODE_CONDITION);
            $$->left = $2;
            $$->op = OP_ODD;
        }
    | expression EQ expression
        {
            $$ = new_node(NODE_CONDITION);
            $$->left = $1;
            $$->right = $3;
            $$->op = OP_EQ;
        }
    | expression NEQ expression
        {
            $$ = new_node(NODE_CONDITION);
            $$->left = $1;
            $$->right = $3;
            $$->op = OP_NEQ;
        }
    | expression LT expression
        {
            $$ = new_node(NODE_CONDITION);
            $$->left = $1;
            $$->right = $3;
            $$->op = OP_LT;
        }
    | expression LTE expression
        {
            $$ = new_node(NODE_CONDITION);
            $$->left = $1;
            $$->right = $3;
            $$->op = OP_LTE;
        }
    | expression GT expression
        {
            $$ = new_node(NODE_CONDITION);
            $$->left = $1;
            $$->right = $3;
            $$->op = OP_GT;
        }
    | expression GTE expression
        {
            $$ = new_node(NODE_CONDITION);
            $$->left = $1;
            $$->right = $3;
            $$->op = OP_GTE;
        }
    ;

expression
    : term                      { $$ = $1; }
    | PLUS term                 { $$ = $2; }
    | MINUS term
        {
            $$ = new_node(NODE_BINARY_OP);
            $$->left = new_number(-1);
            $$->right = $2;
            $$->op = OP_MULT;  // not the most efficient
        }
    | expression PLUS term
        {
            $$ = new_node(NODE_BINARY_OP);
            $$->left = $1;
            $$->right = $3;
            $$->op = OP_PLUS;
        }
    | expression MINUS term
        {
            $$ = new_node(NODE_BINARY_OP);
            $$->left = $1;
            $$->right = $3;
            $$->op = OP_MINUS;
        }
    ;

term
    : factor                    { $$ = $1; }
    | term MULT factor
        {
            $$ = new_node(NODE_BINARY_OP);
            $$->left = $1;
            $$->right = $3;
            $$->op = OP_MULT;
        }
    | term DIV factor
        {
            $$ = new_node(NODE_BINARY_OP);
            $$->left = $1;
            $$->right = $3;
            $$->op = OP_DIV;
        }
    ;

factor
    : IDENT                     { $$ = new_ident($1); }
    | NUM                       { $$ = new_number($1); }
    | LPAREN expression RPAREN  { $$ = $2; }
    ;

%%
/* Epilogue */

void yyerror(const char *s) {
    fprintf(stderr, "ERROR line %d: %s\n", yylineno, s);
}
