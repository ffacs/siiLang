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
    *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::INTEGER, integer("1"))})
  );
  std::string case2 = "1; 2;";
  EXPECT_EQ(
    *parse_from_string(case2), 
    *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::INTEGER, integer("1")),
      astNode(ASTNodeType::INTEGER, integer("2"))})
  );
  std::string case3 = "var1; var2;";
  EXPECT_EQ(
    *parse_from_string(case3), 
    *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::VARIABLE, variable("var1")),
      astNode(ASTNodeType::VARIABLE, variable("var2"))})
  );
  std::string case4 = "(var1);";
  EXPECT_EQ(
    *parse_from_string(case4), 
    *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::VARIABLE, variable("var1"))})
  );
}

TEST(Parser, ArithmeticUnary) {
  std::string case1 = "-1;";
  EXPECT_EQ(
    *parse_from_string(case1), 
    *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::NEG, hyphen(), {
        astNode(ASTNodeType::INTEGER, integer("1"))
        })
      })
  );
  std::string case2 = "-1; -2;";
  EXPECT_EQ(
    *parse_from_string(case2), 
    *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::NEG, hyphen(), {
        astNode(ASTNodeType::INTEGER, integer("1"))
      }),
      astNode(ASTNodeType::NEG, hyphen(), {
        astNode(ASTNodeType::INTEGER, integer("2"))
      })
    })
  );
  std::string case3 = "-var1; -var2;";
  EXPECT_EQ(
    *parse_from_string(case3), 
    *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::NEG, hyphen(), {
        astNode(ASTNodeType::VARIABLE, variable("var1"))
      }),
      astNode(ASTNodeType::NEG, hyphen(), {
        astNode(ASTNodeType::VARIABLE, variable("var2"))
      })
    })
  );
  std::string case4 = "(-var1);";
  EXPECT_EQ(
    *parse_from_string(case4), 
    *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::NEG, hyphen(), {
        astNode(ASTNodeType::VARIABLE, variable("var1"))
      })
    })
  );
}

TEST(Parser, ArithmeticMulAndDiv) {
  std::string case1 = "+1 * -2;";
  EXPECT_EQ(
    *parse_from_string(case1), 
    *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::MUL, asterisk(), {
        astNode(ASTNodeType::INTEGER, integer("1")),
        astNode(ASTNodeType::NEG, hyphen(), {
          astNode(ASTNodeType::INTEGER, integer("2")),
        })
      })})
  );
  std::string case2 = "1 * 2;2 * 2;";
  EXPECT_EQ(
    *parse_from_string(case2), 
    *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::MUL, asterisk(), {
        astNode(ASTNodeType::INTEGER, integer("1")),
        astNode(ASTNodeType::INTEGER, integer("2")),
      }),
      astNode(ASTNodeType::MUL, plus(), {
        astNode(ASTNodeType::INTEGER, integer("2")),
        astNode(ASTNodeType::INTEGER, integer("1")),
      })})
  );
  std::string case3 = "+1 * +2 * -3;";
  EXPECT_EQ(
    *parse_from_string(case3), 
    *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::MUL, asterisk(), {
        astNode(ASTNodeType::MUL, asterisk(), {
          astNode(ASTNodeType::INTEGER, integer("1")),
          astNode(ASTNodeType::INTEGER, integer("2")),}),
        astNode(ASTNodeType::NEG, hyphen(), {
          astNode(ASTNodeType::INTEGER, integer("3")),})
        })
    })
  );
  std::string case4 = "1 / 2;";
  EXPECT_EQ(
    *parse_from_string(case4), 
    *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::DIV, slash(), {
        astNode(ASTNodeType::INTEGER, integer("1")),
        astNode(ASTNodeType::INTEGER, integer("2")),
      })})
  );
  std::string case5 = "1 / 2 * 3;";
  EXPECT_EQ(
    *parse_from_string(case5), 
    *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::MUL, asterisk(), {
        astNode(ASTNodeType::DIV, slash(), {
          astNode(ASTNodeType::INTEGER, integer("1")),
          astNode(ASTNodeType::INTEGER, integer("2")),}),
        astNode(ASTNodeType::INTEGER, integer("3")),})
    })
  );
}

TEST(Parser, ArithmeticAddAndSub) {
  std::string case1 = "1 + 2;";
  EXPECT_EQ(
    *parse_from_string(case1), 
    *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::ADD, plus(), {
        astNode(ASTNodeType::INTEGER, integer("1")),
        astNode(ASTNodeType::INTEGER, integer("2")),
      })})
  );
  std::string case2 = "1 + 2;2 + 2;";
  EXPECT_EQ(
    *parse_from_string(case2), 
    *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::ADD, plus(), {
        astNode(ASTNodeType::INTEGER, integer("1")),
        astNode(ASTNodeType::INTEGER, integer("2")),
      }),
      astNode(ASTNodeType::ADD, plus(), {
        astNode(ASTNodeType::INTEGER, integer("2")),
        astNode(ASTNodeType::INTEGER, integer("1")),
      })})
  );
  std::string case3 = "1 + 2 + 3;";
  EXPECT_EQ(
    *parse_from_string(case3), 
    *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::ADD, plus(), {
        astNode(ASTNodeType::ADD, plus(), {
          astNode(ASTNodeType::INTEGER, integer("1")),
          astNode(ASTNodeType::INTEGER, integer("2")),}),
        astNode(ASTNodeType::INTEGER, integer("3")),})
    })
  );
  std::string case4 = "1 - 2;";
  EXPECT_EQ(
    *parse_from_string(case4), 
    *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::SUB, hyphen(), {
        astNode(ASTNodeType::INTEGER, integer("1")),
        astNode(ASTNodeType::INTEGER, integer("2")),
      })})
  );
  std::string case5 = "1 - 2 + 3;";
  EXPECT_EQ(
    *parse_from_string(case5), 
    *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::ADD, plus(), {
        astNode(ASTNodeType::SUB, hyphen(), {
          astNode(ASTNodeType::INTEGER, integer("1")),
          astNode(ASTNodeType::INTEGER, integer("2")),}),
        astNode(ASTNodeType::INTEGER, integer("3")),})
    })
  );
  std::string case6 = "1 - (2 * (var1 + 3)) + (1 + var2);";
  EXPECT_EQ(
   *parse_from_string(case6),
   *astNode(ASTNodeType::STATEMENTS, nullptr, {
      astNode(ASTNodeType::ADD, plus(), {
        astNode(ASTNodeType::SUB, hyphen(), {
          astNode(ASTNodeType::INTEGER, integer("1")),
          astNode(ASTNodeType::MUL, asterisk(), {
            astNode(ASTNodeType::INTEGER, variable("2")),
            astNode(ASTNodeType::ADD, plus(), {
              astNode(ASTNodeType::VARIABLE, variable("var1")),
              astNode(ASTNodeType::INTEGER, variable("3")),
            })
          })
        }),
        astNode(ASTNodeType::ADD, plus(), {
          astNode(ASTNodeType::INTEGER, integer("1")),
          astNode(ASTNodeType::VARIABLE, variable("var2")),})
      })
    })
  );
}

