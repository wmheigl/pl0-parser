#include <gtest/gtest.h>
#include <sstream>
#include <string>

extern "C" {
#include "ast.h"
}

class ASTTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Called before each test
    }

    void TearDown() override {
        // Called after each test
    }

    // Helper function to capture AST printing output
    std::string capture_ast_output(Node* node, int depth = 0) {
        char* buffer = nullptr;
        size_t size = 0;
        FILE* memstream = open_memstream(&buffer, &size);
        
        fprint_ast(memstream, node, depth);
        fclose(memstream);
        
        std::string result(buffer);
        free(buffer);
        return result;
    }

    // Helper to create a simple program AST
    Node* create_sample_program() {
        // Program: CONST x = 42; VAR y; BEGIN y := x END.
        Node* program = new_node(NODE_PROGRAM);
        Node* block = new_node(NODE_BLOCK);
        
        // CONST x = 42
        Node* const_decl = new_node(NODE_CONST_DECL);
        Node* const_ident = new_ident("x");
        Node* const_value = new_number(42);
        const_decl->left = const_ident;
        const_decl->right = const_value;
        
        // VAR y
        Node* var_decl = new_node(NODE_VAR_DECL);
        Node* var_ident = new_ident("y");
        var_decl->left = var_ident;
        
        // BEGIN y := x END
        Node* compound = new_node(NODE_COMPOUND);
        Node* assign = new_node(NODE_ASSIGN);
        Node* y_ident = new_ident("y");
        Node* x_ident = new_ident("x");
        assign->left = y_ident;
        assign->right = x_ident;
        compound->left = assign;
        
        // Link everything together
        block->left = const_decl;
        block->right = var_decl;
        var_decl->next = compound;
        program->left = block;
        
        return program;
    }
};

// Basic node creation tests
TEST_F(ASTTest, CreateNode) {
    Node* node = new_node(NODE_PROGRAM);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->type, NODE_PROGRAM);
    EXPECT_EQ(node->left, nullptr);
    EXPECT_EQ(node->right, nullptr);
    EXPECT_EQ(node->next, nullptr);
    free(node);
}

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

// Test operator string conversion
TEST_F(ASTTest, OperatorToString) {
    Node* node = new_node(NODE_BINARY_OP);
    node->op = OP_PLUS;
    EXPECT_EQ(to_string(node->op), "PLUS");
    node->op = OP_MINUS;
    EXPECT_EQ(to_string(node->op), "MINUS");
    node->op = OP_MULT;
    EXPECT_EQ(to_string(node->op), "MULT");
    node->op = OP_DIV;
    EXPECT_EQ(to_string(node->op), "DIV");
    node->op = OP_ODD;
    EXPECT_EQ(to_string(node->op), "ODD");
    node->op = OP_EQ;
    EXPECT_EQ(to_string(node->op), "EQ");
    node->op = OP_NEQ;
    EXPECT_EQ(to_string(node->op), "NEQ");
    node->op = OP_LT;
    EXPECT_EQ(to_string(node->op), "LT");
    node->op = OP_LTE;
    EXPECT_EQ(to_string(node->op), "LTE");
    node->op = OP_GT;
    EXPECT_EQ(to_string(node->op), "GT");
    node->op = OP_GTE;
    EXPECT_EQ(to_string(node->op), "GTE");
    free(node);
}

// Test complex AST construction and printing
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
    
    // Verify structure
    EXPECT_EQ(if_node->type, NODE_IF);
    EXPECT_EQ(if_node->left->type, NODE_CONDITION);
    EXPECT_EQ(if_node->right->type, NODE_ASSIGN);
    EXPECT_EQ(condition->op, OP_GT);
    EXPECT_EQ(binary_op->op, OP_PLUS);
    
    // Verify output
    std::string output = capture_ast_output(if_node);
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

// Test full program construction
TEST_F(ASTTest, FullProgramConstruction) {
    Node* program = create_sample_program();
    
    // Verify program structure
    EXPECT_EQ(program->type, NODE_PROGRAM);
    EXPECT_EQ(program->left->type, NODE_BLOCK);
    
    Node* block = program->left;
    EXPECT_EQ(block->left->type, NODE_CONST_DECL);
    EXPECT_EQ(block->right->type, NODE_VAR_DECL);
    EXPECT_EQ(block->right->next->type, NODE_COMPOUND);
    
    // Verify constant declaration
    Node* const_decl = block->left;
    EXPECT_STREQ(const_decl->left->name, "x");
    EXPECT_EQ(const_decl->right->value, 42);
    
    // Verify variable declaration
    Node* var_decl = block->right;
    EXPECT_STREQ(var_decl->left->name, "y");
    
    // Verify compound statement
    Node* compound = var_decl->next;
    Node* assign = compound->left;
    EXPECT_EQ(assign->type, NODE_ASSIGN);
    EXPECT_STREQ(assign->left->name, "y");
    EXPECT_STREQ(assign->right->name, "x");
    
    // Verify AST printing
    std::string output = capture_ast_output(program);
    EXPECT_NE(output.find("Program"), std::string::npos);
    EXPECT_NE(output.find("Block"), std::string::npos);
    EXPECT_NE(output.find("Const Declaration"), std::string::npos);
    EXPECT_NE(output.find("Var Declaration"), std::string::npos);
    EXPECT_NE(output.find("Compound Statement"), std::string::npos);
    EXPECT_NE(output.find("Assignment"), std::string::npos);
    
    // Cleanup the entire tree
    free(compound->left->left->name);  // y in assignment
    free(compound->left->right->name); // x in assignment
    free(compound->left);              // assignment node
    free(compound);                    // compound node
    free(var_decl->left->name);       // y in var declaration
    free(var_decl->left);             // var ident node
    free(var_decl);                   // var declaration node
    free(const_decl->left->name);     // x in const declaration
    free(const_decl->left);           // const ident node
    free(const_decl->right);          // const value node
    free(const_decl);                 // const declaration node
    free(block);                      // block node
    free(program);                    // program node
}

// Test node list operations
TEST_F(ASTTest, NodeListOperations) {
    // Create a list of variable declarations
    Node* var1 = new_node(NODE_VAR_DECL);
    var1->left = new_ident("x");
    Node* var2 = new_node(NODE_VAR_DECL);
    var2->left = new_ident("y");
    Node* var3 = new_node(NODE_VAR_DECL);
    var3->left = new_ident("z");
    
    var1->next = var2;
    var2->next = var3;
    
    // Test list traversal
    Node* current = var1;
    std::vector<std::string> names;
    while (current) {
        names.push_back(current->left->name);
        current = current->next;
    }
    
    EXPECT_EQ(names.size(), 3);
    EXPECT_EQ(names[0], "x");
    EXPECT_EQ(names[1], "y");
    EXPECT_EQ(names[2], "z");
    
    // Cleanup
    free(var3->left->name);
    free(var3->left);
    free(var3);
    free(var2->left->name);
    free(var2->left);
    free(var2);
    free(var1->left->name);
    free(var1->left);
    free(var1);
}

// Test error handling for empty nodes
TEST_F(ASTTest, PrintEmptyNode) {
    std::string output = capture_ast_output(nullptr);
    EXPECT_TRUE(output.empty());
}
