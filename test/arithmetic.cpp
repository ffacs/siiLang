#include "gtest/gtest.h"
#include "parser.h"

static ASTNodePtr parse_from_string(const std::string& str) {
  std::stringstream ss(str);
  auto parser = create_parser(ss, std::cerr);
  return parser->work();
}

TEST(Parser, ArithmeticPrimary) {
  std::string case1 = "1;";
  EXPECT_EQ(
    *parse_from_string(case1), 
    *ASTNode::statements({ASTNode::integer(Token::integer("1"))})
  );
  std::string case2 = "1; 2;";
  EXPECT_EQ(
    *parse_from_string(case2), 
    *ASTNode::statements({ASTNode::integer(Token::integer("1")), 
                          ASTNode::integer(Token::integer("2"))})
  );
  std::string case3 = "var1; var2;";
  EXPECT_EQ(
    *parse_from_string(case3), 
    *ASTNode::statements({ASTNode::variable(Token::variable("var1")), 
                          ASTNode::variable(Token::variable("var2"))})
  );
  std::string case4 = "(var1);";
  EXPECT_EQ(
    *parse_from_string(case4), 
    *ASTNode::statements({ASTNode::variable(Token::variable("var1"))})
  );
}

TEST(Parser, ArithmeticUnary) {
  std::string case1 = "-1;";
  EXPECT_EQ(
    *parse_from_string(case1), 
    *ASTNode::statements({
      ASTNode::negtive(ASTNode::integer(Token::integer("1")))
    })
  );
  std::string case2 = "-1; -2;";
  EXPECT_EQ(
    *parse_from_string(case2), 
    *ASTNode::statements({
      ASTNode::negtive(ASTNode::integer(Token::integer("1"))),
      ASTNode::negtive(ASTNode::integer(Token::integer("2")))
    })
  );
  std::string case3 = "-var1; -var2;";
  EXPECT_EQ(
    *parse_from_string(case3), 
    *ASTNode::statements({
      ASTNode::negtive(ASTNode::variable(Token::variable("var1"))),
      ASTNode::negtive(ASTNode::variable(Token::variable("var2")))
    })
  );
  std::string case4 = "(-var1);";
  EXPECT_EQ(
    *parse_from_string(case4), 
    *ASTNode::statements({
      ASTNode::negtive(ASTNode::variable(Token::variable("var1"))),
    })
  );
}

TEST(Parser, ArithmeticMulAndDiv) {
  std::string case1 = "+1 * -2;";
  EXPECT_EQ(
    *parse_from_string(case1), 
    *ASTNode::statements({
      ASTNode::multiply(
        ASTNode::integer(Token::integer("1")),
        ASTNode::negtive(ASTNode::integer(Token::integer("1")))
      )
    })
  );
  std::string case2 = "1 * 2;2 * 2;";
  EXPECT_EQ(
    *parse_from_string(case2), 
    *ASTNode::statements({
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
  std::string case3 = "+1 * +2 * -3;";
  EXPECT_EQ(
    *parse_from_string(case3), 
    *ASTNode::statements({
      ASTNode::multiply(
        ASTNode::multiply(
          ASTNode::integer(Token::integer("1")),
          ASTNode::integer(Token::integer("1"))), 
        ASTNode::negtive(ASTNode::integer(Token::integer("1"))))
    })
  );
  std::string case4 = "1 / 2;";
  EXPECT_EQ(
    *parse_from_string(case4), 
    *ASTNode::statements({
      ASTNode::divide(
        ASTNode::integer(Token::integer("1")),
        ASTNode::integer(Token::integer("2")))
    })
  );
  std::string case5 = "1 / 2 * 3;";
  EXPECT_EQ(
    *parse_from_string(case5), 
    *ASTNode::statements({
      ASTNode::multiply(
        ASTNode::divide(
          ASTNode::integer(Token::integer("1")),
          ASTNode::integer(Token::integer("2"))), 
        ASTNode::integer(Token::integer("3")))
    })
  );
}

TEST(Parser, ArithmeticAddAndSub) {
  std::string case1 = "1 + 2;";
  EXPECT_EQ(
    *parse_from_string(case1), 
    *ASTNode::statements({
      ASTNode::add(
        ASTNode::integer(Token::integer("1")),
        ASTNode::integer(Token::integer("2")))
    })
  );
  std::string case2 = "1 + 2;2 + 2;";
  EXPECT_EQ(
    *parse_from_string(case2), 
    *ASTNode::statements({
      ASTNode::add(
        ASTNode::integer(Token::integer("1")),
        ASTNode::integer(Token::integer("2"))),
      ASTNode::add(
        ASTNode::integer(Token::integer("2")),
        ASTNode::integer(Token::integer("2"))),
    })
  );
  std::string case3 = "1 + 2 + 3;";
  EXPECT_EQ(
    *parse_from_string(case3), 
    *ASTNode::statements({
      ASTNode::add(
        ASTNode::add(
          ASTNode::integer(Token::integer("1")),
          ASTNode::integer(Token::integer("2"))), 
        ASTNode::integer(Token::integer("3")))
    })
  );
  std::string case4 = "1 - 2;";
  EXPECT_EQ(
    *parse_from_string(case4), 
    *ASTNode::statements({
      ASTNode::subtract(
        ASTNode::integer(Token::integer("1")),
        ASTNode::integer(Token::integer("2")))
    })
  );
  std::string case5 = "1 - 2 + 3;";
  EXPECT_EQ(
    *parse_from_string(case5), 
    *ASTNode::statements({
      ASTNode::add(
        ASTNode::subtract(
          ASTNode::integer(Token::integer("1")),
          ASTNode::integer(Token::integer("2"))), 
        ASTNode::integer(Token::integer("3")))
    })
  );
  std::string case6 = "1 - (2 * (var1 + 3)) + (1 + var2);";
  EXPECT_EQ(
   *parse_from_string(case6),
    *ASTNode::statements({
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
  std::string case1 = "1 == 2;";
  EXPECT_EQ(
    *parse_from_string(case1),
    *ASTNode::statements({
      ASTNode::equal(
        ASTNode::integer(Token::integer("1")),
        ASTNode::integer(Token::integer("2")))
      })
  );
  std::string case2 = "1 != 2;";
  EXPECT_EQ(
    *parse_from_string(case2),
    *ASTNode::statements({
      ASTNode::not_equal(
        ASTNode::integer(Token::integer("1")),
        ASTNode::integer(Token::integer("2")))
      })
  );
  std::string case3 = "1 > 2;";
  EXPECT_EQ(
    *parse_from_string(case3),
    *ASTNode::statements({
      ASTNode::less_than(
        ASTNode::integer(Token::integer("2")),
        ASTNode::integer(Token::integer("1")))
      })
  );
  std::string case4 = "1 >= 2;";
  EXPECT_EQ(
    *parse_from_string(case4),
    *ASTNode::statements({
      ASTNode::less_equal(
        ASTNode::integer(Token::integer("2")),
        ASTNode::integer(Token::integer("1")))
      })
  );
  std::string case5 = "1 < 2;";
  EXPECT_EQ(
    *parse_from_string(case5),
    *ASTNode::statements({
      ASTNode::less_than(
        ASTNode::integer(Token::integer("1")),
        ASTNode::integer(Token::integer("2")))
      })
  );
  std::string case6 = "1 <= 2;";
  EXPECT_EQ(
    *parse_from_string(case6),
    *ASTNode::statements({
      ASTNode::less_equal(
        ASTNode::integer(Token::integer("1")),
        ASTNode::integer(Token::integer("2")))
      })
  );
  std::string case7 = "1 + 2 <= 2;";
  EXPECT_EQ(
    *parse_from_string(case7),
    *ASTNode::statements({
      ASTNode::less_equal(
        ASTNode::add(
          ASTNode::integer(Token::integer("1")),
          ASTNode::integer(Token::integer("2"))),
        ASTNode::integer(Token::integer("2")))
      })
  );
  std::string case8 = "1 * 2 <= 2;";
  EXPECT_EQ(
    *parse_from_string(case8),
    *ASTNode::statements({
      ASTNode::less_equal(
        ASTNode::multiply(
          ASTNode::integer(Token::integer("1")),
          ASTNode::integer(Token::integer("2"))),
        ASTNode::integer(Token::integer("2")))
      })
  );
}
