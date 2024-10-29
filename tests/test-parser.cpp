#include <gtest/gtest.h>
#include <cstring>
extern "C" {
#include "parser.tab.h"
}

extern "C" int yyparse(void);
extern "C" struct yy_buffer_state* yy_scan_string(const char*);

class ParserTest : public ::testing::Test {
   protected:
      void SetUp() override {
         // This method will be called before each test
      }

      void TearDown() override {
         // This method will be called after each test
      }

      void test_parser(const char* input) {
         yy_scan_string(input);
         int result = yyparse();
         ASSERT_EQ(result, 0);
      }
};

TEST_F(ParserTest, EmptyProgram) {
   test_parser(".");
}

TEST_F(ParserTest, Constants) {
   test_parser("CONST a = 10, b = 12, c = 23;.");
}

TEST_F(ParserTest, Variables) {
   test_parser("VAR x, y, z;.");
}

TEST_F(ParserTest, Procedures) {
   test_parser("PROCEDURE p; ;."); // a procedure can have an empty block
   test_parser("PROCEDURE p; ; PROCEDURE p; ;.");
}

TEST_F(ParserTest, Assignment) {
   test_parser("x := 3.");
   test_parser("x := abc.");
}

TEST_F(ParserTest, Call) {
   test_parser("CALL square.");
}

TEST_F(ParserTest, Read) {
   test_parser("READ x.");
}

TEST_F(ParserTest, WRITE) {
   test_parser("WRITE y.");
}

TEST_F(ParserTest, BeginEnd) {
   test_parser("BEGIN END.");
   test_parser("BEGIN x := 5 END.");
}

TEST_F(ParserTest, IfThen) {
   test_parser("IF x = 0 THEN.");
   test_parser("IF x = 0 THEN x := 1.");
}

TEST_F(ParserTest, WhileDo) {
   test_parser("WHILE x < 5 DO x := x + 1.");
}

TEST_F(ParserTest, Conditions) {
   test_parser("IF ODD 5 THEN x := 1.");
   test_parser("IF x = 5 THEN x := 0.");
   test_parser("IF x # 5 THEN x := 0.");
   test_parser("IF x < 5 THEN x := 0.");
   test_parser("IF x <= 5 THEN x := 0.");
   test_parser("IF x > 5 THEN x := 0.");
   test_parser("IF x >= 5 THEN x := 0.");
}

TEST_F(ParserTest, Expressions) {
   test_parser(".");
   test_parser("x := +y.");
   test_parser("x := -7.");
   test_parser("x := a + (-7)."); // it would be a syntax error w/o parens
   test_parser("x := -7 - b.");
   test_parser("x := 3 * 5.");
   test_parser("x := 6 / 2.");
   test_parser("x := b*b - 4*a*c.");
   test_parser("x := (-b - (b*b - 4*a*c)) / (2*a).");
}

TEST_F(ParserTest, ComplexProgram) {
   const char* input = 
      R"(VAR x, y, z, q, r, n, f;

         PROCEDURE multiply;
         VAR a, b;
         BEGIN
           a := x;
           b := y;
           z := 0;
           WHILE b > 0 DO
           BEGIN
             IF ODD b THEN z := z + a;
             a := 2 * a;
             b := b / 2
           END
         END;

         PROCEDURE divide;
         VAR w;
         BEGIN
           r := x;
           q := 0;
           w := y;
           WHILE w <= r DO w := 2 * w;
           WHILE w > y DO
           BEGIN
             q := 2 * q;
             w := w / 2;
             IF w <= r THEN
             BEGIN
               r := r - w;
               q := q + 1
             END
           END
         END;

         PROCEDURE gcd;
         VAR f, g;
         BEGIN
           f := x;
           g := y;
           WHILE f # g DO
           BEGIN
             IF f < g THEN g := g - f;
             IF g < f THEN f := f - g
           END;
           z := f
         END;

         PROCEDURE fact;
         BEGIN
           IF n > 1 THEN
           BEGIN
             f := n * f;
             n := n - 1;
             CALL fact
           END
         END;

         BEGIN
           READ x; READ y; CALL multiply; WRITE z;
           READ x; READ y; CALL divide; WRITE q; WRITE r;
           READ x; READ y; CALL gcd; WRITE z;
           READ n; f := 1; CALL fact; WRITE f
         END.)";
   test_parser(input);
}

