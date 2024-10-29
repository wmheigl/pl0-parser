#include <gtest/gtest.h>
extern "C" {
#include "ast.h"
}

class ASTTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

// Test node creation
TEST_F(ASTTest, CreateNode) {
    Node* node = new_node(NODE_PROGRAM);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->type, NODE_PROGRAM);
    EXPECT_EQ(node->left, nullptr);
    EXPECT_EQ(node->right, nullptr);
    EXPECT_EQ(node->next, nullptr);
    free(node);
}

// Test identifier node creation
TEST_F(ASTTest, CreateIdentifier) {
    const char* test_name = "test_var";
    Node* node = new_ident(test_name);
    
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->type, NODE_IDENT);
    ASSERT_NE(node->name, nullptr);
    EXPECT_STREQ(node->name, test_name);
    EXPECT_EQ(node->left, nullptr);
    EXPECT_EQ(node->right, nullptr);
    
    free(node->name);
    free(node);
}

// Test number node creation
TEST_F(ASTTest, CreateNumber) {
    int test_value = 42;
    Node* node = new_number(test_value);
    
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->type, NODE_NUMBER);
    EXPECT_EQ(node->value, test_value);
    EXPECT_EQ(node->left, nullptr);
    EXPECT_EQ(node->right, nullptr);
    
    free(node);
}

// Test print_ast with captured output
TEST_F(ASTTest, PrintAST) {
    // Create a test AST: const x = 42;
    Node* program = new_node(NODE_PROGRAM);
    Node* block = new_node(NODE_BLOCK);
    Node* const_decl = new_node(NODE_CONST_DECL);
    Node* ident = new_ident("x");
    Node* number = new_number(42);
    
    program->left = block;
    block->left = const_decl;
    const_decl->left = ident;
    const_decl->right = number;

    // Capture stdout
    testing::internal::CaptureStdout();
    
    // Print the AST
    print_ast(program, 0);
    
    // Get the output
    std::string output = testing::internal::GetCapturedStdout();
    
    // Verify the output contains expected strings
    EXPECT_NE(output.find("Program"), std::string::npos);
    EXPECT_NE(output.find("Block"), std::string::npos);
    EXPECT_NE(output.find("Const Declaration: x = 42"), std::string::npos);
    
    // Cleanup
    free(ident->name);
    free(ident);
    free(number);
    free(const_decl);
    free(block);
    free(program);
}

// Test complex AST construction
TEST_F(ASTTest, ComplexASTConstruction) {
    // Create AST for: if (x > 5) then y := x + 1
    Node* if_node = new_node(NODE_IF);
    Node* condition = new_node(NODE_CONDITION);
    Node* x_ident = new_ident("x");
    Node* number_5 = new_number(5);
    Node* assign = new_node(NODE_ASSIGN);
    Node* y_ident = new_ident("y");
    Node* binary_op = new_node(NODE_BINARY_OP);
    Node* x_ident2 = new_ident("x");
    Node* number_1 = new_number(1);
    
    // Build the AST
    condition->left = x_ident;
    condition->right = number_5;
    condition->op = OP_GT;
    
    binary_op->left = x_ident2;
    binary_op->right = number_1;
    binary_op->op = OP_PLUS;
    
    assign->left = y_ident;
    assign->right = binary_op;
    
    if_node->left = condition;
    if_node->right = assign;
    
    // Verify the structure
    EXPECT_EQ(if_node->type, NODE_IF);
    EXPECT_EQ(if_node->left->type, NODE_CONDITION);
    EXPECT_EQ(if_node->right->type, NODE_ASSIGN);
    EXPECT_EQ(condition->op, OP_GT);
    EXPECT_EQ(binary_op->op, OP_PLUS);
    
    // Capture and verify output
    testing::internal::CaptureStdout();
    print_ast(if_node, 0);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("If Statement"), std::string::npos);
    EXPECT_NE(output.find("Condition: GT"), std::string::npos);
    EXPECT_NE(output.find("Assignment"), std::string::npos);
    EXPECT_NE(output.find("Binary Operation: PLUS"), std::string::npos);
    
    // Cleanup
    free(x_ident->name);
    free(x_ident);
    free(number_5);
    free(condition);
    free(y_ident->name);
    free(y_ident);
    free(x_ident2->name);
    free(x_ident2);
    free(number_1);
    free(binary_op);
    free(assign);
    free(if_node);
}
