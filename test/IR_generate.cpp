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
      "  %0 = 1 * 2;", 
      IRStringGenerate(
        ASTNode::compound_statement({
          ASTNode::multiply(
            ASTNode::integer("1"),
            ASTNode::integer("2"))
      }))
  );
  EXPECT_EQ(
      "  %0 = %var1 * %var2;", 
      IRStringGenerate(
        ASTNode::compound_statement({
          ASTNode::multiply(
            ASTNode::variable("var1"),
            ASTNode::variable("var2"))
      }))
  );
}

TEST(IRGenerator, Divide) {
  EXPECT_EQ(
      "  %0 = 1 / 2;", 
      IRStringGenerate(
        ASTNode::compound_statement({
          ASTNode::divide(
            ASTNode::integer("1"),
            ASTNode::integer("2"))
      }))
  );
  EXPECT_EQ(
      "  %0 = 1 / 2;", 
      IRStringGenerate(
        ASTNode::compound_statement({
          ASTNode::divide(
            ASTNode::integer("1"),
            ASTNode::integer("2"))
      }))
  );
}

TEST(IRGenerator, Addition) {
  EXPECT_EQ(
      "  %0 = 1 + 2;", 
      IRStringGenerate(
        ASTNode::compound_statement({
          ASTNode::add(
            ASTNode::integer("1"),
            ASTNode::integer("2"))
      }))
  );
}

TEST(IRGenerator, Subtraction) {
  EXPECT_EQ(
      "  %0 = 1 + 2;", 
      IRStringGenerate(
        ASTNode::compound_statement({
          ASTNode::add(
            ASTNode::integer("1"),
            ASTNode::integer("2"))
      }))
  );
}

TEST(IRGenerator, Equal) {
  EXPECT_EQ(
      "  %0 = 1 == 2;", 
      IRStringGenerate(
        ASTNode::compound_statement({
          ASTNode::equal(
            ASTNode::integer("1"),
            ASTNode::integer("2"))
      }))
  );
}

TEST(IRGenerator, NotEqual) {
  EXPECT_EQ(
      "  %0 = 1 != 2;", 
      IRStringGenerate(
        ASTNode::compound_statement({
          ASTNode::not_equal(
            ASTNode::integer("1"),
            ASTNode::integer("2"))
      }))
  );
}

TEST(IRGenerator, LessThan) {
  EXPECT_EQ(
      "  %0 = 1 < 2;", 
      IRStringGenerate(
        ASTNode::compound_statement({
          ASTNode::less_than(
            ASTNode::integer("1"),
            ASTNode::integer("2"))
      }))
  );
}

TEST(IRGenerator, LessEqual) {
  EXPECT_EQ(
      "  %0 = 1 <= 2;", 
      IRStringGenerate(
        ASTNode::compound_statement({
          ASTNode::less_equal(
            ASTNode::integer("1"),
            ASTNode::integer("2"))
      }))
  );
}

TEST(IRGenerator, Negtive) {
  EXPECT_EQ(
      "  %0 = -1;", 
      IRStringGenerate(
        ASTNode::compound_statement({
          ASTNode::negtive(
            ASTNode::integer("1"))
      }))
  );
}

TEST(IRGenerator, Statements) {
  EXPECT_EQ(
      "  %0 = 1 + 2;\n"
      "  %1 = 1 - 2;",
      IRStringGenerate(
        ASTNode::compound_statement({
          ASTNode::add(
            ASTNode::integer("1"),
            ASTNode::integer("2")),
        ASTNode::subtract(
          ASTNode::integer("1"),
          ASTNode::integer("2")),
      }))
  );
}

TEST(IRGenerator, Recursive) {
  EXPECT_EQ(
      "  %0 = 1 * 2;\n"
      "  %1 = -3;\n"
      "  %2 = %0 * %1;", 
      IRStringGenerate(
        ASTNode::compound_statement({
          ASTNode::multiply(
            ASTNode::multiply(
              ASTNode::integer("1"),
              ASTNode::integer("2")),
            ASTNode::negtive(
              ASTNode::integer("3"))
          )
      }))
  );
}

TEST(IRGenerator, Assign) {
  EXPECT_EQ(
      "  %var2 = 2;\n"
      "  %var1 = %var2;",
      IRStringGenerate(
        ASTNode::compound_statement({
          ASTNode::assign(
            ASTNode::variable("var1"),
            ASTNode::assign(
              ASTNode::variable("var2"),
              ASTNode::integer("2"))
          )
      }))
  );
  EXPECT_EQ(
      "  %0 = %var1 + %var1;\n"
      "  %var2 = %0;\n"
      "  %var1 = %var2;",
      IRStringGenerate(
        ASTNode::compound_statement({
          ASTNode::assign(
            ASTNode::variable("var1"),
            ASTNode::assign(
              ASTNode::variable("var2"),
              ASTNode::add(
                ASTNode::variable("var1"), 
                ASTNode::variable("var1")))
          )
      }))
  );
}

TEST(IRGenerator, CompoundStatement) {
  EXPECT_EQ(
    "  %var1 = %var2;\n"
    "  %var1 = 2;",
    IRStringGenerate(
      ASTNode::compound_statement({
        ASTNode::compound_statement({
          ASTNode::assign(
            ASTNode::variable("var1"),
            ASTNode::variable("var2"))
        }),
        ASTNode::compound_statement({
          ASTNode::compound_statement({
            ASTNode::assign(
              ASTNode::variable("var1"),
              ASTNode::integer("2"))
          })
        }),
        ASTNode::compound_statement({})
      })
    )
  );
}

TEST(IRGenerator, SelectStatement) {
  EXPECT_EQ(
    "  if %var1 goto Label.0;\n"
    "  goto Label.1;\n"
    "Label.0:\n"
    "  %var1 = 1;\n"
    "  goto Label.2;\n"
    "Label.1:\n"
    "  if %var2 goto Label.3;\n"
    "  goto Label.4;\n"
    "Label.3:\n"
    "  %var2 = 2;\n"
    "  goto Label.5;\n"
    "Label.4:\n"
    "  %var1 = 3;\n"
    "Label.5:\n"
    "Label.2:\n"
    "  nope;",
    IRStringGenerate(
      ASTNode::compound_statement({
        ASTNode::if_else(
          ASTNode::variable("var1"),
          ASTNode::compound_statement({
            ASTNode::assign(
              ASTNode::variable("var1"),
              ASTNode::integer("1")),
          }),
          ASTNode::if_else(
            ASTNode::variable("var2"), 
            ASTNode::compound_statement({
              ASTNode::assign(
                ASTNode::variable("var2"),
                ASTNode::integer("2"))
            }),
            ASTNode::compound_statement({
              ASTNode::assign(
                ASTNode::variable("var1"),
                ASTNode::integer("3"))
            })
          )
        )
      })
    )
  );
  EXPECT_EQ(
    "  if %var1 goto Label.0;\n"
    "  goto Label.1;\n"
    "Label.0:\n"
    "  %var1 = 1;\n"
    "  goto Label.2;\n"
    "Label.1:\n"
    "  if %var2 goto Label.3;\n"
    "  goto Label.4;\n"
    "Label.3:\n"
    "  %var2 = 2;\n"
    "  goto Label.5;\n"
    "Label.4:\n"
    "  %var1 = 3;\n"
    "Label.5:\n"
    "Label.2:\n"
    "  %var3 = %var4;",
    IRStringGenerate(
      ASTNode::compound_statement({
        ASTNode::if_else(
          ASTNode::variable("var1"),
          ASTNode::compound_statement({
            ASTNode::assign(
              ASTNode::variable("var1"),
              ASTNode::integer("1")),
          }),
          ASTNode::if_else(
            ASTNode::variable("var2"), 
            ASTNode::compound_statement({
              ASTNode::assign(
                ASTNode::variable("var2"),
                ASTNode::integer("2"))
            }),
            ASTNode::compound_statement({
              ASTNode::assign(
                ASTNode::variable("var1"),
                ASTNode::integer("3"))
            })
          )
        ),
        ASTNode::assign(
          ASTNode::variable("var3"),
          ASTNode::variable("var4"))
      })
    )
  );
}
