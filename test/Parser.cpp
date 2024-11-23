#include "gtest/gtest.h"
#include "parser.h"

static ASTNodePtr parse_from_string(const std::string& str) {
  std::stringstream ss(str);
  auto parser = CreateParser(ss);
  return parser->work();
}

TEST(Parser, ArithmeticPrimary) {
  std::string case1 = "{1;}";
  EXPECT_EQ(
    *parse_from_string(case1), 
    *ASTNode::compound_statement({ASTNode::integer(Token::integer("1"))})
  );
  std::string case2 = "{1; 2;}";
  EXPECT_EQ(
    *parse_from_string(case2), 
    *ASTNode::compound_statement({ASTNode::integer(Token::integer("1")), 
                          ASTNode::integer(Token::integer("2"))})
  );
  std::string case3 = "{var1; var2;}";
  EXPECT_EQ(
    *parse_from_string(case3), 
    *ASTNode::compound_statement({ASTNode::variable(Token::variable("var1")), 
                          ASTNode::variable(Token::variable("var2"))})
  );
  std::string case4 = "{(var1);}";
  EXPECT_EQ(
    *parse_from_string(case4), 
    *ASTNode::compound_statement({ASTNode::variable(Token::variable("var1"))})
  );
}

TEST(Parser, ArithmeticUnary) {
  std::string case1 = "{-1;}";
  EXPECT_EQ(
    *parse_from_string(case1), 
    *ASTNode::compound_statement({
      ASTNode::negtive(ASTNode::integer(Token::integer("1")))
    })
  );
  std::string case2 = "{-1; -2;}";
  EXPECT_EQ(
    *parse_from_string(case2), 
    *ASTNode::compound_statement({
      ASTNode::negtive(ASTNode::integer(Token::integer("1"))),
      ASTNode::negtive(ASTNode::integer(Token::integer("2")))
    })
  );
  std::string case3 = "{-var1; -var2;}";
  EXPECT_EQ(
    *parse_from_string(case3), 
    *ASTNode::compound_statement({
      ASTNode::negtive(ASTNode::variable(Token::variable("var1"))),
      ASTNode::negtive(ASTNode::variable(Token::variable("var2")))
    })
  );
  std::string case4 = "{(-var1);}";
  EXPECT_EQ(
    *parse_from_string(case4), 
    *ASTNode::compound_statement({
      ASTNode::negtive(ASTNode::variable(Token::variable("var1"))),
    })
  );
  std::string case5 = "{-(-var1);}";
  EXPECT_EQ(
    *parse_from_string(case5), 
    *ASTNode::compound_statement({
      ASTNode::negtive(
        ASTNode::negtive(
          ASTNode::variable(Token::variable("var1")))),
    })
  );
}

TEST(Parser, ArithmeticMulAndDiv) {
  std::string case1 = "{+1 * -2;}";
  EXPECT_EQ(
    *parse_from_string(case1), 
    *ASTNode::compound_statement({
      ASTNode::multiply(
        ASTNode::integer(Token::integer("1")),
        ASTNode::negtive(ASTNode::integer(Token::integer("1")))
      )
    })
  );
  std::string case2 = "{1 * 2;2 * 2;}";
  EXPECT_EQ(
    *parse_from_string(case2), 
    *ASTNode::compound_statement({
      ASTNode::multiply(
        ASTNode::integer(Token::integer("1")),
        ASTNode::integer(Token::integer("2"))
      ),
      ASTNode::multiply(
        ASTNode::integer(Token::integer("2")),
        ASTNode::integer(Token::integer("2"))
      )
    })
  );
  std::string case3 = "{+1 * +2 * -3;}";
  EXPECT_EQ(
    *parse_from_string(case3), 
    *ASTNode::compound_statement({
      ASTNode::multiply(
        ASTNode::multiply(
          ASTNode::integer(Token::integer("1")),
          ASTNode::integer(Token::integer("1"))), 
        ASTNode::negtive(ASTNode::integer(Token::integer("1"))))
    })
  );
  std::string case4 = "{1 / 2;}";
  EXPECT_EQ(
    *parse_from_string(case4), 
    *ASTNode::compound_statement({
      ASTNode::divide(
        ASTNode::integer(Token::integer("1")),
        ASTNode::integer(Token::integer("2")))
    })
  );
  std::string case5 = "{1 / 2 * 3;}";
  EXPECT_EQ(
    *parse_from_string(case5), 
    *ASTNode::compound_statement({
      ASTNode::multiply(
        ASTNode::divide(
          ASTNode::integer(Token::integer("1")),
          ASTNode::integer(Token::integer("2"))), 
        ASTNode::integer(Token::integer("3")))
    })
  );
}

TEST(Parser, ArithmeticAddAndSub) {
  std::string case1 = "{1 + 2;}";
  EXPECT_EQ(
    *parse_from_string(case1), 
    *ASTNode::compound_statement({
      ASTNode::add(
        ASTNode::integer(Token::integer("1")),
        ASTNode::integer(Token::integer("2")))
    })
  );
  std::string case2 = "{1 + 2;2 + 2;}";
  EXPECT_EQ(
    *parse_from_string(case2), 
    *ASTNode::compound_statement({
      ASTNode::add(
        ASTNode::integer(Token::integer("1")),
        ASTNode::integer(Token::integer("2"))),
      ASTNode::add(
        ASTNode::integer(Token::integer("2")),
        ASTNode::integer(Token::integer("2"))),
    })
  );
  std::string case3 = "{1 + 2 + 3;}";
  EXPECT_EQ(
    *parse_from_string(case3), 
    *ASTNode::compound_statement({
      ASTNode::add(
        ASTNode::add(
          ASTNode::integer(Token::integer("1")),
          ASTNode::integer(Token::integer("2"))), 
        ASTNode::integer(Token::integer("3")))
    })
  );
  std::string case4 = "{1 - 2;}";
  EXPECT_EQ(
    *parse_from_string(case4), 
    *ASTNode::compound_statement({
      ASTNode::subtract(
        ASTNode::integer(Token::integer("1")),
        ASTNode::integer(Token::integer("2")))
    })
  );
  std::string case5 = "{1 - 2 + 3;}";
  EXPECT_EQ(
    *parse_from_string(case5), 
    *ASTNode::compound_statement({
      ASTNode::add(
        ASTNode::subtract(
          ASTNode::integer(Token::integer("1")),
          ASTNode::integer(Token::integer("2"))), 
        ASTNode::integer(Token::integer("3")))
    })
  );
  std::string case6 = "{1 - (2 * (var1 + 3)) + (1 + var2);}";
  EXPECT_EQ(
   *parse_from_string(case6),
    *ASTNode::compound_statement({
      ASTNode::add(
        ASTNode::subtract(
          ASTNode::integer(Token::integer("1")),
          ASTNode::multiply(
            ASTNode::integer(Token::integer("2")),
            ASTNode::add(
              ASTNode::variable(Token::variable("var1")), 
              ASTNode::integer(Token::integer("3"))))
        ),
        ASTNode::add(
          ASTNode::integer(Token::integer("1")),
          ASTNode::variable(Token::variable("var2")))
        )
      })
  );
}

TEST(Parser, ArithmeticRelation) {
  std::string case1 = "{1 == 2;}";
  EXPECT_EQ(
    *parse_from_string(case1),
    *ASTNode::compound_statement({
      ASTNode::equal(
        ASTNode::integer(Token::integer("1")),
        ASTNode::integer(Token::integer("2")))
      })
  );
  std::string case2 = "{1 != 2;}";
  EXPECT_EQ(
    *parse_from_string(case2),
    *ASTNode::compound_statement({
      ASTNode::not_equal(
        ASTNode::integer(Token::integer("1")),
        ASTNode::integer(Token::integer("2")))
      })
  );
  std::string case3 = "{1 > 2;}";
  EXPECT_EQ(
    *parse_from_string(case3),
    *ASTNode::compound_statement({
      ASTNode::less_than(
        ASTNode::integer(Token::integer("2")),
        ASTNode::integer(Token::integer("1")))
      })
  );
  std::string case4 = "{1 >= 2;}";
  EXPECT_EQ(
    *parse_from_string(case4),
    *ASTNode::compound_statement({
      ASTNode::less_equal(
        ASTNode::integer(Token::integer("2")),
        ASTNode::integer(Token::integer("1")))
      })
  );
  std::string case5 = "{1 < 2;}";
  EXPECT_EQ(
    *parse_from_string(case5),
    *ASTNode::compound_statement({
      ASTNode::less_than(
        ASTNode::integer(Token::integer("1")),
        ASTNode::integer(Token::integer("2")))
      })
  );
  std::string case6 = "{1 <= 2;}";
  EXPECT_EQ(
    *parse_from_string(case6),
    *ASTNode::compound_statement({
      ASTNode::less_equal(
        ASTNode::integer(Token::integer("1")),
        ASTNode::integer(Token::integer("2")))
      })
  );
  std::string case7 = "{1 < 2 <= 2;}";
  EXPECT_EQ(
    *parse_from_string(case7),
    *ASTNode::compound_statement({
      ASTNode::less_equal(
        ASTNode::less_than(
          ASTNode::integer(Token::integer("1")),
          ASTNode::integer(Token::integer("2"))),
        ASTNode::integer(Token::integer("2")))
      })
  );
}

TEST(Parser, Assignment) {
  EXPECT_EQ(
    *parse_from_string("{1 = 2 = 3;}"),
    *ASTNode::compound_statement({
      ASTNode::assign(
        ASTNode::integer(Token::integer("1")),
        ASTNode::assign(
          ASTNode::integer(Token::integer("2")),
          ASTNode::integer(Token::integer("3"))))
      })
  );
  EXPECT_EQ(
    *parse_from_string("{1 == 2 = var2 = 3 + var3;}"),
    *ASTNode::compound_statement({
      ASTNode::assign(
        ASTNode::equal(
          ASTNode::integer(Token::integer("1")), 
          ASTNode::integer(Token::integer("2"))),
        ASTNode::assign(
          ASTNode::variable(Token::variable("var2")),
          ASTNode::add(
            ASTNode::integer(Token::integer("3")), 
            ASTNode::variable(Token::variable("var3")))))
    })
  );
}

TEST(Parser, CompoundStatement) {
  EXPECT_EQ(
    *parse_from_string("{ {var1;} {{var2;}} {} }"),
    *ASTNode::compound_statement({
      ASTNode::compound_statement({ASTNode::variable(Token::variable("var1"))}),
      ASTNode::compound_statement({
        ASTNode::compound_statement(
          {ASTNode::variable(Token::variable("var2"))})
      }),
      ASTNode::compound_statement({})
    })
  );
}
