#include <gtest/gtest.h>
#include <string>
#include <cstdio>

extern "C" {
#include "ast.h"
#include "type_check.h"
#include "semantic.h"
extern int yyparse(void);
extern struct yy_buffer_state* yy_scan_string(const char*);
extern Node* ast_root;
}

class AnalysisTest : public ::testing::Test {
protected:
    void SetUp() override {
        type_ctx = create_type_context();
        sem_ctx = create_semantic_context();
    }

    void TearDown() override {
        if (type_ctx) free_type_context(type_ctx);
        if (sem_ctx) free_semantic_context(sem_ctx);
    }

    bool parse_and_analyze(const char* input) {
        yy_scan_string(input);
        if (yyparse() != 0) return false;
        
        Type type = check_type(type_ctx, ast_root);
        if (type == TYPE_ERROR) return false;
        
        return analyze_semantics(sem_ctx, ast_root);
    }

    TypeContext* type_ctx;
    SemanticContext* sem_ctx;
};

// Basic program tests
TEST_F(AnalysisTest, EmptyProgram) {
    EXPECT_TRUE(parse_and_analyze("."));
}

TEST_F(AnalysisTest, SimpleConstant) {
    EXPECT_TRUE(parse_and_analyze("CONST x = 42;."));
}

TEST_F(AnalysisTest, SimpleVariable) {
    EXPECT_TRUE(parse_and_analyze("VAR x; BEGIN x := 42 END."));
}

// Type checking tests
TEST_F(AnalysisTest, InvalidAssignment) {
    EXPECT_FALSE(parse_and_analyze("VAR x; BEGIN x := y END."));
    EXPECT_STREQ(sem_ctx->error_msg, "Undefined identifier 'y'");
}

TEST_F(AnalysisTest, AssignToConstant) {
    EXPECT_FALSE(parse_and_analyze("CONST x = 42; BEGIN x := 10 END."));
    EXPECT_STREQ(sem_ctx->error_msg, "Cannot assign to constant 'x'");
}

TEST_F(AnalysisTest, ValidArithmetic) {
    EXPECT_TRUE(parse_and_analyze(
        "VAR x, y;"
        "BEGIN"
        "  x := 42;"
        "  y := x + 10 * 2"
        "END."));
}

// Semantic analysis tests
TEST_F(AnalysisTest, ProcedureCall) {
    EXPECT_TRUE(parse_and_analyze(
        "VAR x;"
        "PROCEDURE p;"
        "BEGIN"
        "   x := 42"
        "END;"
        "CALL p"
        "."));
}

TEST_F(AnalysisTest, UndefinedProcedure) {
    EXPECT_FALSE(parse_and_analyze("BEGIN CALL p END."));
    EXPECT_STREQ(sem_ctx->error_msg, "Undefined procedure 'p'");
}

TEST_F(AnalysisTest, Scoping) {
    EXPECT_TRUE(parse_and_analyze(
        "VAR x;"
        "PROCEDURE p;"
        "VAR x;"
        "BEGIN"
        "   x := 42"
        "END;"
        "BEGIN"
        "  x := 10;"
        "  CALL p"
        "END."));
}

// Control structure tests
TEST_F(AnalysisTest, ValidWhileLoop) {
    EXPECT_TRUE(parse_and_analyze(
        "VAR x;"
        "BEGIN"
        "  x := 10;"
        "  WHILE x > 0 DO"
        "    x := x - 1"
        "END."));
}

TEST_F(AnalysisTest, ValidIfStatement) {
    EXPECT_TRUE(parse_and_analyze(
        "VAR x;"
        "BEGIN"
        "  x := 10;"
        "  IF x = 10 THEN"
        "    x := 0"
        "END."));
}

// Complex program tests
TEST_F(AnalysisTest, ComplexProgram) {
    EXPECT_TRUE(parse_and_analyze(
        "CONST max = 100;"
        "VAR n, sum, i;"
        "PROCEDURE compute;"
        "  BEGIN"
        "    sum := 0;"
        "    i := 1;"
        "    WHILE i <= n DO"
        "    BEGIN"
        "      sum := sum + i;"
        "      i := i + 1"
        "    END"
        "  END;"
        "BEGIN"
        "  n := max;"
        "  CALL compute"
        "END."));
}

