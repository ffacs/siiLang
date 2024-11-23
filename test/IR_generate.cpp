#include "gtest/gtest.h"
#include "IR_generator.h"
#include <sstream>

static std::string IRStringGenerate(const ASTNodePtr root) {
  auto generator = CreateIRGenerator(std::move(root));
  auto IR_list = generator->work();
  std::stringstream result_builder;
  for (size_t i = 0; i < IR_list.size(); i++) {
    result_builder << IR_list[i]->to_string();
    if (i != IR_list.size() - 1) {
      result_builder << "\n";
    }
  }
  return result_builder.str();
}

TEST(IRGenerator, Multiply) {
  EXPECT_EQ(
      "%0 = 1 * 2;", 
      IRStringGenerate(
        ASTNode::statements({
          ASTNode::multiply(
            ASTNode::integer(Token::integer("1")),
            ASTNode::integer(Token::integer("2")))
      }))
  );
  EXPECT_EQ(
      "%0 = var1 * var2;", 
      IRStringGenerate(
        ASTNode::statements({
          ASTNode::multiply(
            ASTNode::variable(Token::variable("var1")),
            ASTNode::variable(Token::variable("var2")))
      }))
  );
}

TEST(IRGenerator, Divide) {
  EXPECT_EQ(
      "%0 = 1 / 2;", 
      IRStringGenerate(
        ASTNode::statements({
          ASTNode::divide(
            ASTNode::integer(Token::integer("1")),
            ASTNode::integer(Token::integer("2")))
      }))
  );
  EXPECT_EQ(
      "%0 = 1 / 2;", 
      IRStringGenerate(
        ASTNode::statements({
          ASTNode::divide(
            ASTNode::integer(Token::integer("1")),
            ASTNode::integer(Token::integer("2")))
      }))
  );
}

TEST(IRGenerator, Addition) {
  EXPECT_EQ(
      "%0 = 1 + 2;", 
      IRStringGenerate(
        ASTNode::statements({
          ASTNode::add(
            ASTNode::integer(Token::integer("1")),
            ASTNode::integer(Token::integer("2")))
      }))
  );
}

TEST(IRGenerator, Subtraction) {
  EXPECT_EQ(
      "%0 = 1 + 2;", 
      IRStringGenerate(
        ASTNode::statements({
          ASTNode::add(
            ASTNode::integer(Token::integer("1")),
            ASTNode::integer(Token::integer("2")))
      }))
  );
}

TEST(IRGenerator, Equal) {
  EXPECT_EQ(
      "%0 = 1 == 2;", 
      IRStringGenerate(
        ASTNode::statements({
          ASTNode::equal(
            ASTNode::integer(Token::integer("1")),
            ASTNode::integer(Token::integer("2")))
      }))
  );
}

TEST(IRGenerator, NotEqual) {
  EXPECT_EQ(
      "%0 = 1 != 2;", 
      IRStringGenerate(
        ASTNode::statements({
          ASTNode::not_equal(
            ASTNode::integer(Token::integer("1")),
            ASTNode::integer(Token::integer("2")))
      }))
  );
}

TEST(IRGenerator, LessThan) {
  EXPECT_EQ(
      "%0 = 1 < 2;", 
      IRStringGenerate(
        ASTNode::statements({
          ASTNode::less_than(
            ASTNode::integer(Token::integer("1")),
            ASTNode::integer(Token::integer("2")))
      }))
  );
}

TEST(IRGenerator, LessEqual) {
  EXPECT_EQ(
      "%0 = 1 <= 2;", 
      IRStringGenerate(
        ASTNode::statements({
          ASTNode::less_equal(
            ASTNode::integer(Token::integer("1")),
            ASTNode::integer(Token::integer("2")))
      }))
  );
}

TEST(IRGenerator, Negtive) {
  EXPECT_EQ(
      "%0 = -1;", 
      IRStringGenerate(
        ASTNode::statements({
          ASTNode::negtive(
            ASTNode::integer(Token::integer("1")))
      }))
  );
}

TEST(IRGenerator, Statements) {
  EXPECT_EQ(
      "%0 = 1 + 2;\n"
      "%1 = 1 - 2;",
      IRStringGenerate(
        ASTNode::statements({
          ASTNode::add(
            ASTNode::integer(Token::integer("1")),
            ASTNode::integer(Token::integer("2"))),
        ASTNode::subtract(
          ASTNode::integer(Token::integer("1")),
          ASTNode::integer(Token::integer("2"))),
      }))
  );
}

TEST(IRGenerator, Recursive) {
  EXPECT_EQ(
      "%0 = 1 * 2;\n"
      "%1 = -3;\n"
      "%2 = %0 * %1;", 
      IRStringGenerate(
        ASTNode::statements({
          ASTNode::multiply(
            ASTNode::multiply(
              ASTNode::integer(Token::integer("1")),
              ASTNode::integer(Token::integer("2"))),
            ASTNode::negtive(
              ASTNode::integer(Token::integer("3")))
          )
      }))
  );
}
