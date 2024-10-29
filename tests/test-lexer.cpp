#include <gtest/gtest.h>
#include <cstring>
extern "C" {
#include "parser.tab.h"
}

extern "C" int yylex(void);
extern "C" char* yytext;
extern "C" struct yy_buffer_state* yy_scan_string(const char*);

TEST(LexerTest, Keywords) {
    const char* input =
        "CONST VAR PROCEDURE READ WRITE BEGIN END IF THEN WHILE DO CALL ODD";
    yy_scan_string(input);

    EXPECT_EQ(yylex(), TOK_CONST);
    EXPECT_EQ(yylex(), TOK_VAR);
    EXPECT_EQ(yylex(), TOK_PROC);
    EXPECT_EQ(yylex(), TOK_READ);
    EXPECT_EQ(yylex(), TOK_WRITE);
    EXPECT_EQ(yylex(), TOK_BEGIN);
    EXPECT_EQ(yylex(), TOK_END);
    EXPECT_EQ(yylex(), TOK_IF);
    EXPECT_EQ(yylex(), TOK_THEN);
    EXPECT_EQ(yylex(), TOK_WHILE);
    EXPECT_EQ(yylex(), TOK_DO);
    EXPECT_EQ(yylex(), TOK_CALL);
    EXPECT_EQ(yylex(), TOK_ODD);
}

TEST(LexerTest, OperatorsAndPunctuation) {
    const char* input = ":= = # < <= > >= + - * / ( ) , ; .";
    yy_scan_string(input);

    EXPECT_EQ(yylex(), TOK_ASSIGN);
    EXPECT_EQ(yylex(), TOK_EQ);
    EXPECT_EQ(yylex(), TOK_NEQ);
    EXPECT_EQ(yylex(), TOK_LT);
    EXPECT_EQ(yylex(), TOK_LTE);
    EXPECT_EQ(yylex(), TOK_GT);
    EXPECT_EQ(yylex(), TOK_GTE);
    EXPECT_EQ(yylex(), TOK_PLUS);
    EXPECT_EQ(yylex(), TOK_MINUS);
    EXPECT_EQ(yylex(), TOK_MULT);
    EXPECT_EQ(yylex(), TOK_DIV);
    EXPECT_EQ(yylex(), TOK_LPAREN);
    EXPECT_EQ(yylex(), TOK_RPAREN);
    EXPECT_EQ(yylex(), TOK_COMMA);
    EXPECT_EQ(yylex(), TOK_SEMICOLON);
    EXPECT_EQ(yylex(), TOK_DOT);
}

TEST(LexerTest, NumbersAndIdentifiers) {
    const char* input = "123 square x";
    yy_scan_string(input);
    EXPECT_EQ(yylex(), TOK_NUM);
    EXPECT_STREQ(yytext, "123");
    EXPECT_EQ(yylex(), TOK_IDENT);
    EXPECT_STREQ(yytext, "square");
    EXPECT_EQ(yylex(), TOK_IDENT);
    EXPECT_STREQ(yytext, "x");
}

TEST(LexerTest, WhiteSpace) {
    const char* input = " \t\n";
    yy_scan_string(input);
    EXPECT_EQ(yylex(), 0);
}
