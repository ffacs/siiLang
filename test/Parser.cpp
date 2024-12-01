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
    *ASTNode::compound_statement({ASTNode::integer("1")})
  );
  std::string case2 = "{1; 2;}";
  EXPECT_EQ(
    *parse_from_string(case2), 
    *ASTNode::compound_statement({ASTNode::integer("1"), 
                                  ASTNode::integer("2")})
  );
  std::string case3 = "{var1; var2;}";
  EXPECT_EQ(
    *parse_from_string(case3), 
    *ASTNode::compound_statement({ASTNode::variable("var1"), 
                          ASTNode::variable("var2")})
  );
  std::string case4 = "{(var1);}";
  EXPECT_EQ(
    *parse_from_string(case4), 
    *ASTNode::compound_statement({ASTNode::variable("var1")})
  );
}

TEST(Parser, ArithmeticUnary) {
  std::string case1 = "{-1;}";
  EXPECT_EQ(
    *parse_from_string(case1), 
    *ASTNode::compound_statement({
      ASTNode::negtive(ASTNode::integer("1"))
    })
  );
  std::string case2 = "{-1; -2;}";
  EXPECT_EQ(
    *parse_from_string(case2), 
    *ASTNode::compound_statement({
      ASTNode::negtive(ASTNode::integer("1")),
      ASTNode::negtive(ASTNode::integer("2"))
    })
  );
  std::string case3 = "{-var1; -var2;}";
  EXPECT_EQ(
    *parse_from_string(case3), 
    *ASTNode::compound_statement({
      ASTNode::negtive(ASTNode::variable("var1")),
      ASTNode::negtive(ASTNode::variable("var2"))
    })
  );
  std::string case4 = "{(-var1);}";
  EXPECT_EQ(
    *parse_from_string(case4), 
    *ASTNode::compound_statement({
      ASTNode::negtive(ASTNode::variable("var1")),
    })
  );
  std::string case5 = "{-(-var1);}";
  EXPECT_EQ(
    *parse_from_string(case5), 
    *ASTNode::compound_statement({
      ASTNode::negtive(
        ASTNode::negtive(
          ASTNode::variable("var1"))),
    })
  );
}

TEST(Parser, ArithmeticMulAndDiv) {
  std::string case1 = "{+1 * -2;}";
  EXPECT_EQ(
    *parse_from_string(case1), 
    *ASTNode::compound_statement({
      ASTNode::multiply(
        ASTNode::integer("1"),
        ASTNode::negtive(ASTNode::integer("1"))
      )
    })
  );
  std::string case2 = "{1 * 2;2 * 2;}";
  EXPECT_EQ(
    *parse_from_string(case2), 
    *ASTNode::compound_statement({
      ASTNode::multiply(
        ASTNode::integer("1"),
        ASTNode::integer("2")
      ),
      ASTNode::multiply(
        ASTNode::integer("2"),
        ASTNode::integer("2")
      )
    })
  );
  std::string case3 = "{+1 * +2 * -3;}";
  EXPECT_EQ(
    *parse_from_string(case3), 
    *ASTNode::compound_statement({
      ASTNode::multiply(
        ASTNode::multiply(
          ASTNode::integer("1"),
          ASTNode::integer("1")), 
        ASTNode::negtive(ASTNode::integer("1")))
    })
  );
  std::string case4 = "{1 / 2;}";
  EXPECT_EQ(
    *parse_from_string(case4), 
    *ASTNode::compound_statement({
      ASTNode::divide(
        ASTNode::integer("1"),
        ASTNode::integer("2"))
    })
  );
  std::string case5 = "{1 / 2 * 3;}";
  EXPECT_EQ(
    *parse_from_string(case5), 
    *ASTNode::compound_statement({
      ASTNode::multiply(
        ASTNode::divide(
          ASTNode::integer("1"),
          ASTNode::integer("2")), 
        ASTNode::integer("3"))
    })
  );
}

TEST(Parser, ArithmeticAddAndSub) {
  std::string case1 = "{1 + 2;}";
  EXPECT_EQ(
    *parse_from_string(case1), 
    *ASTNode::compound_statement({
      ASTNode::add(
        ASTNode::integer("1"),
        ASTNode::integer("2"))
    })
  );
  std::string case2 = "{1 + 2;2 + 2;}";
  EXPECT_EQ(
    *parse_from_string(case2), 
    *ASTNode::compound_statement({
      ASTNode::add(
        ASTNode::integer("1"),
        ASTNode::integer("2")),
      ASTNode::add(
        ASTNode::integer("2"),
        ASTNode::integer("2")),
    })
  );
  std::string case3 = "{1 + 2 + 3;}";
  EXPECT_EQ(
    *parse_from_string(case3), 
    *ASTNode::compound_statement({
      ASTNode::add(
        ASTNode::add(
          ASTNode::integer("1"),
          ASTNode::integer("2")), 
        ASTNode::integer("3"))
    })
  );
  std::string case4 = "{1 - 2;}";
  EXPECT_EQ(
    *parse_from_string(case4), 
    *ASTNode::compound_statement({
      ASTNode::subtract(
        ASTNode::integer("1"),
        ASTNode::integer("2"))
    })
  );
  std::string case5 = "{1 - 2 + 3;}";
  EXPECT_EQ(
    *parse_from_string(case5), 
    *ASTNode::compound_statement({
      ASTNode::add(
        ASTNode::subtract(
          ASTNode::integer("1"),
          ASTNode::integer("2")), 
        ASTNode::integer("3"))
    })
  );
  std::string case6 = "{1 - (2 * (var1 + 3)) + (1 + var2);}";
  EXPECT_EQ(
   *parse_from_string(case6),
    *ASTNode::compound_statement({
      ASTNode::add(
        ASTNode::subtract(
          ASTNode::integer("1"),
          ASTNode::multiply(
            ASTNode::integer("2"),
            ASTNode::add(
              ASTNode::variable("var1"), 
              ASTNode::integer("3")))
        ),
        ASTNode::add(
          ASTNode::integer("1"),
          ASTNode::variable("var2"))
        )
      })
  );
}

TEST(Parser, ArithmeticRelation) {
  std::string case1 = "{1 > 2;}";
  EXPECT_EQ(
    *parse_from_string(case1),
    *ASTNode::compound_statement({
      ASTNode::less_than(
        ASTNode::integer("2"),
        ASTNode::integer("1"))
      })
  );
  std::string case2 = "{1 >= 2;}";
  EXPECT_EQ(
    *parse_from_string(case2),
    *ASTNode::compound_statement({
      ASTNode::less_equal(
        ASTNode::integer("2"),
        ASTNode::integer("1"))
      })
  );
  std::string case3 = "{1 < 2;}";
  EXPECT_EQ(
    *parse_from_string(case3),
    *ASTNode::compound_statement({
      ASTNode::less_than(
        ASTNode::integer("1"),
        ASTNode::integer("2"))
      })
  );
  std::string case4 = "{1 <= 2;}";
  EXPECT_EQ(
    *parse_from_string(case4),
    *ASTNode::compound_statement({
      ASTNode::less_equal(
        ASTNode::integer("1"),
        ASTNode::integer("2"))
      })
  );
  std::string case5 = "{1 < 2 <= 2;}";
  EXPECT_EQ(
    *parse_from_string(case5),
    *ASTNode::compound_statement({
      ASTNode::less_equal(
        ASTNode::less_than(
          ASTNode::integer("1"),
          ASTNode::integer("2")),
        ASTNode::integer("2"))
      })
  );
}

TEST(Parser, ArithmeticEquality) {
  std::string case1 = "{1 == 2;}";
  EXPECT_EQ(
    *parse_from_string(case1),
    *ASTNode::compound_statement({
      ASTNode::equal(
        ASTNode::integer("1"),
        ASTNode::integer("2"))
      })
  );
  std::string case2 = "{1 != 2;}";
  EXPECT_EQ(
    *parse_from_string(case2),
    *ASTNode::compound_statement({
      ASTNode::not_equal(
        ASTNode::integer("1"),
        ASTNode::integer("2"))
      })
  );
  std::string case3 = "{1 < 2 != 2;}";
  EXPECT_EQ(
    *parse_from_string(case3),
    *ASTNode::compound_statement({
      ASTNode::not_equal(
        ASTNode::less_than(ASTNode::integer("1"), ASTNode::integer("2")),
        ASTNode::integer("2"))
      })
  );
}

TEST(Parser, Assignment) {
  EXPECT_EQ(
    *parse_from_string("{1 = 2 = 3;}"),
    *ASTNode::compound_statement({
      ASTNode::assign(
        ASTNode::integer("1"),
        ASTNode::assign(
          ASTNode::integer("2"),
          ASTNode::integer("3")))
      })
  );
  EXPECT_EQ(
    *parse_from_string("{1 == 2 = var2 = 3 + var3;}"),
    *ASTNode::compound_statement({
      ASTNode::assign(
        ASTNode::equal(
          ASTNode::integer("1"), 
          ASTNode::integer("2")),
        ASTNode::assign(
          ASTNode::variable("var2"),
          ASTNode::add(
            ASTNode::integer("3"), 
            ASTNode::variable("var3"))))
    })
  );
}

TEST(Parser, CompoundStatement) {
  EXPECT_EQ(
    *parse_from_string("{ {var1;} {{var2;}} {} }"),
    *ASTNode::compound_statement({
      ASTNode::compound_statement({ASTNode::variable("var1")}),
      ASTNode::compound_statement({
        ASTNode::compound_statement(
          {ASTNode::variable("var2")})
      }),
      ASTNode::compound_statement({})
    })
  );
}

TEST(Parser, SelectStatement) {
  EXPECT_EQ(
    *parse_from_string("{if(var1) { var1 = 1;}}"),
    *ASTNode::compound_statement({
      ASTNode::if_else(
        ASTNode::variable("var1"),
        ASTNode::compound_statement({
          ASTNode::assign(
            ASTNode::variable("var1"),
            ASTNode::integer("1")),
        }),
        nullptr)
    })
  );
  EXPECT_EQ(
    *parse_from_string("{if(var1) { var1 = 1;} else {var1=2;}}"),
    *ASTNode::compound_statement({
      ASTNode::if_else(
        ASTNode::variable("var1"),
        ASTNode::compound_statement({
          ASTNode::assign(
            ASTNode::variable("var1"),
            ASTNode::integer("1")),
        }),
        ASTNode::compound_statement({
          ASTNode::assign(
            ASTNode::variable("var1"),
            ASTNode::integer("2")),
        })
      )
    })
  );
  EXPECT_EQ(
    *parse_from_string("{if(var1) { var1 = 1;} else if (var2) {var2=2;} else {var1 = 3;}}"),
    *ASTNode::compound_statement({
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
  );
}

TEST(Parser, IterationStatement) {
  EXPECT_EQ(
    *parse_from_string("{for(;;) {}}"),
    *ASTNode::compound_statement({
      ASTNode::for_loop(
        ASTNode::empty(),
        ASTNode::empty(),
        ASTNode::empty(),
        ASTNode::compound_statement({ }))
    })
  );
  EXPECT_EQ(
    *parse_from_string("{for(var1;var2;var3) {}}"),
    *ASTNode::compound_statement({
      ASTNode::for_loop(
        ASTNode::variable("var1"),
        ASTNode::variable("var2"),
        ASTNode::variable("var3"),
        ASTNode::compound_statement({ }))
    })
  );
  EXPECT_EQ(
    *parse_from_string("{for(var1; var2; var3) { var1 = 3; } }"),
    *ASTNode::compound_statement({
      ASTNode::for_loop(
        ASTNode::variable("var1"),
        ASTNode::variable("var2"),
        ASTNode::variable("var3"),
        ASTNode::compound_statement({
          ASTNode::assign(ASTNode::variable("var1"), ASTNode::integer("3"))
        }))
    })
  );
  EXPECT_EQ(
    *parse_from_string("{while(var1) {}}"),
    *ASTNode::compound_statement({
      ASTNode::while_loop(
        ASTNode::variable("var1"),
        ASTNode::compound_statement({ }))
    })
  );
  EXPECT_EQ(
    *parse_from_string("{while(var1) { var1 = 3; }}"),
    *ASTNode::compound_statement({
      ASTNode::while_loop(
        ASTNode::variable("var1"),
        ASTNode::compound_statement({
          ASTNode::assign(ASTNode::variable("var1"), ASTNode::integer("3"))
        }))
    })
  );
  EXPECT_ANY_THROW(parse_from_string("{while() {}}"));
  EXPECT_EQ(
    *parse_from_string("{do {var1 = 3;} while(var1);}"),
    *ASTNode::compound_statement({
      ASTNode::do_while(
        ASTNode::compound_statement({
          ASTNode::assign(ASTNode::variable("var1"), ASTNode::integer("3"))
        }),
        ASTNode::variable("var1"))
    })
  );
  EXPECT_EQ(
    *parse_from_string("{do ; while(var1 = 3);}"),
    *ASTNode::compound_statement({
      ASTNode::do_while(
        ASTNode::empty(),
        ASTNode::assign(ASTNode::variable("var1"), ASTNode::integer("3")))
    })
  );
  EXPECT_ANY_THROW(parse_from_string("{do while(var1);}"));
  EXPECT_ANY_THROW(parse_from_string("{do ;while();}"));
  EXPECT_ANY_THROW(parse_from_string("{do ;while()}"));
}

TEST(Parser, Declaration) {
  EXPECT_ANY_THROW(parse_from_string("{int (*);}"));
  EXPECT_ANY_THROW(parse_from_string("{int (*)();}"));
  EXPECT_ANY_THROW(parse_from_string("{int *();}"));
  EXPECT_ANY_THROW(parse_from_string("{int *;}"));
  EXPECT_NO_THROW(parse_from_string("{int ;}"));
  EXPECT_EQ(
    *parse_from_string("{int a;}"), 
    *ASTNode::compound_statement({
      ASTNode::declaration({
        ASTNode::single_declaration(
          Type::basic(TypeKind::INT),
          ASTNode::variable("a"),
          nullptr)
      })
    })
  );
  EXPECT_EQ(
    *parse_from_string("{int a, b;}"), 
    *ASTNode::compound_statement({
      ASTNode::declaration({
        ASTNode::single_declaration(
          Type::basic(TypeKind::INT),
          ASTNode::variable("a"),
          nullptr),
        ASTNode::single_declaration(
          Type::basic(TypeKind::INT),
          ASTNode::variable("b"),
          nullptr)
      })
    })
  );
  EXPECT_EQ(
    *parse_from_string("{int **a, *b;}"), 
    *ASTNode::compound_statement({
      ASTNode::declaration({
        ASTNode::single_declaration(
          Type::poniter(Type::poniter(Type::basic(TypeKind::INT))),
          ASTNode::variable("a"),
          nullptr),
        ASTNode::single_declaration(
          Type::poniter(Type::basic(TypeKind::INT)),
          ASTNode::variable("b"),
          nullptr)
      })
    })
  );
  EXPECT_EQ(
    *parse_from_string("{int a[4], *b;}"), 
    *ASTNode::compound_statement({
      ASTNode::declaration({
        ASTNode::single_declaration(
          Type::array(Type::basic(TypeKind::INT), 4),
          ASTNode::variable("a"),
          nullptr),
        ASTNode::single_declaration(
          Type::poniter(Type::basic(TypeKind::INT)),
          ASTNode::variable("b"),
          nullptr)
      })
    })
  );
  EXPECT_EQ(
    *parse_from_string("{int a[4][3], *b;}"), 
    *ASTNode::compound_statement({
      ASTNode::declaration({
        ASTNode::single_declaration(
          Type::array(Type::array(Type::basic(TypeKind::INT), 3), 4),
          ASTNode::variable("a"),
          nullptr),
        ASTNode::single_declaration(
          Type::poniter(Type::basic(TypeKind::INT)),
          ASTNode::variable("b"),
          nullptr)
      })
    })
  );
  EXPECT_EQ(
    *parse_from_string("{int (*a)[4][3], *b;}"), 
    *ASTNode::compound_statement({
      ASTNode::declaration({
        ASTNode::single_declaration(
          Type::poniter(Type::array(Type::array(Type::basic(TypeKind::INT), 3), 4)),
          ASTNode::variable("a"),
          nullptr),
        ASTNode::single_declaration(
          Type::poniter(Type::basic(TypeKind::INT)),
          ASTNode::variable("b"),
          nullptr)
      })
    })
  );
  EXPECT_EQ(
    *parse_from_string("{int a(), *b;}"), 
    *ASTNode::compound_statement({
      ASTNode::declaration({
        ASTNode::single_declaration(
          Type::function(Type::basic(TypeKind::INT), {}),
          ASTNode::variable("a"),
          nullptr),
        ASTNode::single_declaration(
          Type::poniter(Type::basic(TypeKind::INT)),
          ASTNode::variable("b"),
          nullptr)
      })
    })
  );
  EXPECT_EQ(
    *parse_from_string("{int (*a)(), *b;}"), 
    *ASTNode::compound_statement({
      ASTNode::declaration({
        ASTNode::single_declaration(
          Type::poniter(Type::function(Type::basic(TypeKind::INT), {})),
          ASTNode::variable("a"),
          nullptr),
        ASTNode::single_declaration(
          Type::poniter(Type::basic(TypeKind::INT)),
          ASTNode::variable("b"),
          nullptr)
      })
    })
  );
  EXPECT_EQ(
    *parse_from_string("{int *(*a[4])(), *b;}"), 
    *ASTNode::compound_statement({
      ASTNode::declaration({
        ASTNode::single_declaration(
          Type::array(Type::poniter(Type::function(Type::poniter(Type::basic(TypeKind::INT)), {})), 4),
          ASTNode::variable("a"),
          nullptr),
        ASTNode::single_declaration(
          Type::poniter(Type::basic(TypeKind::INT)),
          ASTNode::variable("b"),
          nullptr)
      })
    })
  );
  EXPECT_EQ(
    *parse_from_string("{int *(*a[4][3]())()[3], *b;}"), 
    *ASTNode::compound_statement({
      ASTNode::declaration({
        ASTNode::single_declaration(
          Type::array(Type::array(Type::function(Type::poniter(Type::function(Type::array(Type::poniter(Type::basic(TypeKind::INT)), 3), {})), {}) ,3) ,4),
          ASTNode::variable("a"),
          nullptr),
        ASTNode::single_declaration(
          Type::poniter(Type::basic(TypeKind::INT)),
          ASTNode::variable("b"),
          nullptr)
      })
    })
  );
  EXPECT_EQ(
    *parse_from_string("{int *a = c = b + 10;}"), 
    *ASTNode::compound_statement({
      ASTNode::declaration({
        ASTNode::single_declaration(
          Type::poniter(Type::basic(TypeKind::INT)),
          ASTNode::variable("a"),
          ASTNode::assign(ASTNode::variable("c"), ASTNode::add(ASTNode::variable("b"), ASTNode::integer("10")))
        ),
      })
    })
  );
}
