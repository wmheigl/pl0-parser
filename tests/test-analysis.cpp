#include <gtest/gtest.h>
#include <string>
#include <sstream>

extern "C" {
#include "ast.h"
#include "semantic.h"
extern int yyparse(void);
extern struct yy_buffer_state* yy_scan_string(const char*);
extern Node* ast_root;
}

class SemanticAnalysisTest : public ::testing::Test {
protected:
    void SetUp() override {
        sem_ctx = create_semantic_context();
        ASSERT_NE(sem_ctx, nullptr);
    }

    void TearDown() override {
        if (sem_ctx) {
            free_semantic_context(sem_ctx);
        }
    }

    bool parse_and_analyze(const std::string& input) {
        yy_scan_string(input.c_str());
        if (yyparse() != 0) return false;
        return analyze_semantics(sem_ctx, ast_root);
    }

    // Helper function to capture symbol table output
    std::string get_symbol_table() {
        char* buffer = nullptr;
        size_t size = 0;
        FILE* memstream = open_memstream(&buffer, &size);
        
        dump_symbol_table(sem_ctx, memstream);
        fclose(memstream);
        
        std::string result(buffer);
        free(buffer);
        return result;
    }

    SemanticContext* sem_ctx;
};

/* NOTE: Strings that serve as test programs will be streamed into
 *       an array in memory. There needs to be empty space between
 *       and after keywords or a '\n' (unless there is a ';') so
 *       that parsing works correctly.
 */

// Basic Declaration Tests
TEST_F(SemanticAnalysisTest, ConstantDeclaration) {
    ASSERT_TRUE(parse_and_analyze("CONST x = 42;."));
    std::string symbols = get_symbol_table();
    EXPECT_TRUE(symbols.find("x") != std::string::npos);
    EXPECT_TRUE(symbols.find("constant") != std::string::npos);
    EXPECT_TRUE(symbols.find("42") != std::string::npos);
}

TEST_F(SemanticAnalysisTest, MultipleConstants) {
    ASSERT_TRUE(parse_and_analyze("CONST x = 42, y = 10;."));
    std::string symbols = get_symbol_table();
    EXPECT_TRUE(symbols.find("x") != std::string::npos);
    EXPECT_TRUE(symbols.find("y") != std::string::npos);
    EXPECT_TRUE(symbols.find("42") != std::string::npos);
    EXPECT_TRUE(symbols.find("10") != std::string::npos);
}

TEST_F(SemanticAnalysisTest, VariableDeclaration) {
    ASSERT_TRUE(parse_and_analyze("VAR x;."));
    std::string symbols = get_symbol_table();
    EXPECT_TRUE(symbols.find("x") != std::string::npos);
    EXPECT_TRUE(symbols.find("variable") != std::string::npos);
}

TEST_F(SemanticAnalysisTest, MultipleVariables) {
    ASSERT_TRUE(parse_and_analyze("VAR x, y, z;."));
    std::string symbols = get_symbol_table();
    EXPECT_TRUE(symbols.find("x") != std::string::npos);
    EXPECT_TRUE(symbols.find("y") != std::string::npos);
    EXPECT_TRUE(symbols.find("z") != std::string::npos);
}

// Procedure Tests
TEST_F(SemanticAnalysisTest, ProcedureDeclaration) {
    ASSERT_TRUE(parse_and_analyze(
        "PROCEDURE p;"
        "BEGIN END;"
        "."));
    std::string symbols = get_symbol_table();
    EXPECT_TRUE(symbols.find("p") != std::string::npos);
    EXPECT_TRUE(symbols.find("procedure") != std::string::npos);
}

TEST_F(SemanticAnalysisTest, NestedProcedures) {
    ASSERT_TRUE(parse_and_analyze(
        "PROCEDURE outer;"
        "  PROCEDURE inner;"
        "  BEGIN"
        "  END;"
        "BEGIN END;"
        "."));
    std::string symbols = get_symbol_table();
    EXPECT_TRUE(symbols.find("outer") != std::string::npos);
    EXPECT_TRUE(symbols.find("inner") != std::string::npos);
}

// Scope Tests
TEST_F(SemanticAnalysisTest, VariableScoping) {
    ASSERT_TRUE(parse_and_analyze(
        "VAR x;"
        "PROCEDURE p;"
        "  VAR x;"
        "BEGIN"
        "  x := 1"
        "END;"
        "BEGIN"
        "  x := 2"
        "END."));
    std::string symbols = get_symbol_table();
    // Should see x listed twice
    size_t count = 0;
    size_t pos = 0;
    while ((pos = symbols.find("x", pos)) != std::string::npos) {
        count++;
        pos++;
    }
    EXPECT_GE(count, 2);
}

// Error Cases
TEST_F(SemanticAnalysisTest, DuplicateConstant) {
    ASSERT_FALSE(parse_and_analyze(
        "CONST x = 1, x = 2;."));
    EXPECT_TRUE(std::string(sem_ctx->error_msg).find("already declared") != std::string::npos);
}

TEST_F(SemanticAnalysisTest, DuplicateVariable) {
    ASSERT_FALSE(parse_and_analyze(
        "VAR x, x;."));
    EXPECT_TRUE(std::string(sem_ctx->error_msg).find("already declared") != std::string::npos);
}

TEST_F(SemanticAnalysisTest, DuplicateProcedure) {
    ASSERT_FALSE(parse_and_analyze(
        "PROCEDURE p; BEGIN END;"
        "PROCEDURE p; BEGIN END;."));
    EXPECT_TRUE(std::string(sem_ctx->error_msg).find("already declared") != std::string::npos);
}

TEST_F(SemanticAnalysisTest, UndefinedVariable) {
    ASSERT_FALSE(parse_and_analyze(
        "BEGIN x := 1 END."));
    EXPECT_TRUE(std::string(sem_ctx->error_msg).find("Undefined identifier") != std::string::npos);
}

TEST_F(SemanticAnalysisTest, AssignToConstant) {
    ASSERT_FALSE(parse_and_analyze(
        "CONST x = 1;"
        "BEGIN x := 2 END."));
    EXPECT_TRUE(std::string(sem_ctx->error_msg).find("Cannot assign to constant") != std::string::npos);
}

TEST_F(SemanticAnalysisTest, AssignToProcedure) {
    ASSERT_FALSE(parse_and_analyze(
        "PROCEDURE p; BEGIN END;"
        "BEGIN p := 1 END."));
    EXPECT_TRUE(std::string(sem_ctx->error_msg).find("Cannot assign to procedure") != std::string::npos);
}

// Compound Statement Tests
TEST_F(SemanticAnalysisTest, CompoundStatement) {
    ASSERT_TRUE(parse_and_analyze(
        "VAR x, y;"
        "BEGIN"
        "  x := 1;"
        "  y := 2"
        "END."));
}

// Procedure Call Tests
TEST_F(SemanticAnalysisTest, ValidProcedureCall) {
    ASSERT_TRUE(parse_and_analyze(
        "PROCEDURE p; BEGIN END;"
        "BEGIN CALL p END."));
}

TEST_F(SemanticAnalysisTest, UndefinedProcedureCall) {
    ASSERT_FALSE(parse_and_analyze(
        "BEGIN CALL p END."));
    EXPECT_TRUE(std::string(sem_ctx->error_msg).find("Undefined procedure") != std::string::npos);
}

TEST_F(SemanticAnalysisTest, CallNonProcedure) {
    ASSERT_FALSE(parse_and_analyze(
        "VAR x;"
        "BEGIN CALL x END."));
    EXPECT_TRUE(std::string(sem_ctx->error_msg).find("is not a procedure") != std::string::npos);
}

// Read Statement Tests
TEST_F(SemanticAnalysisTest, ValidRead) {
    ASSERT_TRUE(parse_and_analyze(
        "VAR x;"
        "BEGIN READ x END."));
}

TEST_F(SemanticAnalysisTest, ReadIntoConstant) {
    ASSERT_FALSE(parse_and_analyze(
        "CONST x = 1;"
        "BEGIN READ x END."));
    EXPECT_TRUE(std::string(sem_ctx->error_msg).find("must be a variable") != std::string::npos);
}

TEST_F(SemanticAnalysisTest, ReadIntoProcedure) {
    ASSERT_FALSE(parse_and_analyze(
        "PROCEDURE p; BEGIN END;"
        "BEGIN READ p END."));
    EXPECT_TRUE(std::string(sem_ctx->error_msg).find("must be a variable") != std::string::npos);
}

// Complex Program Tests
TEST_F(SemanticAnalysisTest, ComplexProgram) {
    ASSERT_TRUE(parse_and_analyze(
        "CONST max = 100;"
        "VAR n, sum, i;"
        "PROCEDURE compute;"
        "  VAR temp;"
        "  BEGIN"
        "    sum := 0;"
        "    i := 1;"
        "    WHILE i <= n DO"
        "    BEGIN"
        "      temp := i;"
        "      sum := sum + temp;"
        "      i := i + 1"
        "    END"
        "  END;"
        "BEGIN"
        "  n := max;"
        "  CALL compute "
        "END."));
    
    std::string symbols = get_symbol_table();
    // Verify all symbols are present
    EXPECT_TRUE(symbols.find("max") != std::string::npos);
    EXPECT_TRUE(symbols.find("n") != std::string::npos);
    EXPECT_TRUE(symbols.find("sum") != std::string::npos);
    EXPECT_TRUE(symbols.find("i") != std::string::npos);
    EXPECT_TRUE(symbols.find("compute") != std::string::npos);
    EXPECT_TRUE(symbols.find("temp") != std::string::npos);
}

