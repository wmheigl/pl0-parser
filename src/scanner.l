%{
#include <stdio.h>
#include <string.h>
#include "parser.tab.h"

extern void yyerror(const char *s);
%}

%option warn nodefault
%option noyywrap noinput nounput
%option yylineno

%%

[ \t\n]+                { /* Ignore whitespace */ }
"CONST"                 { return TOK_CONST; }
"VAR"                   { return TOK_VAR; }
"PROCEDURE"             { return TOK_PROC; }
"CALL"                  { return TOK_CALL; }
"READ"                  { return TOK_READ; }
"WRITE"                 { return TOK_WRITE; }
"BEGIN"                 { return TOK_BEGIN; }
"END"                   { return TOK_END; }
"IF"                    { return TOK_IF; }
"THEN"                  { return TOK_THEN; }
"WHILE"                 { return TOK_WHILE; }
"DO"                    { return TOK_DO; }
"ODD"                   { return TOK_ODD; }
[0-9]+                  { yylval.value = atoi(yytext); return TOK_NUM; }
[a-zA-Z][a-zA-Z0-9]*    { yylval.name = strdup(yytext); return TOK_IDENT; }
":="                    { return TOK_ASSIGN; }
"="                     { return TOK_EQ; }
"#"                     { return TOK_NEQ; }
"<"                     { return TOK_LT; }
"<="                    { return TOK_LTE; }
">"                     { return TOK_GT; }
">="                    { return TOK_GTE; }
"+"                     { return TOK_PLUS; }
"-"                     { return TOK_MINUS; }
"*"                     { return TOK_MULT; }
"/"                     { return TOK_DIV; }
"("                     { return TOK_LPAREN; }
")"                     { return TOK_RPAREN; }
";"                     { return TOK_SEMICOLON; }
","                     { return TOK_COMMA; }
"."                     { return TOK_DOT; }
.                       { yyerror("Unexpected character"); }

