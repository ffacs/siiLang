#include "gtest/gtest.h"
#include "front/IR_generator.h"
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

TEST(IRGenerator, FunctionDeclaration) {
  EXPECT_EQ(
      "@function1:\n",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function1"),
          ASTNode::compound_statement({ })))
      );
  EXPECT_EQ(
      "@function1:\n"
      "  alloca %0 size 4;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function1"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), nullptr)
          })))
      );
  EXPECT_EQ(
      "@function1:\n"
      "  alloca %0 size 4;\n"
      "  %0 = 1;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function1"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), ASTNode::integer("1"))
          })))
      );
  EXPECT_EQ(
      "@function1:\n"
      "  alloca %0 size 4;\n"
      "  alloca %1 size 4;\n"
      "  %0 = 0;\n"
      "  %1 = 1;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function1"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), ASTNode::integer("0")),
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var2"), ASTNode::integer("1")),
          })))
      );
  EXPECT_EQ(
      "@function1:\n"
      "  alloca %0 size 4;\n"
      "  alloca %1 size 4;\n"
      "  %0 = 0;\n"
      "  %1 = 1;\n"
      "  %0 = %1;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function1"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), ASTNode::integer("0")),
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var2"), ASTNode::integer("1")),
            ASTNode::assign(ASTNode::identifier("var1"), ASTNode::identifier("var2"))
          })))
      );
  EXPECT_EQ(
      "@function1:\n"
      "  alloca %0 size 4;\n"
      "  alloca %1 size 4;\n"
      "  %0 = 0;\n"
      "  %1 = 1;\n"
      "  %2 = 1 + 2;\n"
      "  %3 = %1 + %2;\n"
      "  %0 = %3;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function1"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), ASTNode::integer("0")),
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var2"), ASTNode::integer("1")),
            ASTNode::assign(ASTNode::identifier("var1"), ASTNode::add(ASTNode::identifier("var2"), 
                                                                      ASTNode::add(ASTNode::integer("1"), ASTNode::integer("2"))))
          })))
      );
  EXPECT_EQ(
      "@function1:\n"
      "  alloca %0 size 4;\n"
      "  alloca %1 size 4;\n"
      "  %0 = 0;\n"
      "  %2 = 1 + 2;\n"
      "  %0 = %2;\n"
      "  %1 = 1;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function1"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), ASTNode::integer("0")),
            ASTNode::assign(ASTNode::identifier("var1"), ASTNode::add(ASTNode::integer("1"), ASTNode::integer("2"))),
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var2"), ASTNode::integer("1"))
          })))
      );
}

TEST(IRGenerator, FunctionDeclarationWithParameter) {
  EXPECT_EQ(
      "@function1:\n",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {
              Declarator::Create(Type::basic(TypeKind::INT), "a"), }), "function1"),
          ASTNode::compound_statement({ })))
      );
  EXPECT_THROW(
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT),
            {
              Declarator::Create(Type::basic(TypeKind::INT), "a"), 
              Declarator::Create(Type::basic(TypeKind::INT), "a"), 
            }),
            "function1"),
          ASTNode::compound_statement({ }))),
      std::invalid_argument);
  EXPECT_EQ(
      "@function1:\n"
      "  %0 = 1;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {
              Declarator::Create(Type::basic(TypeKind::INT), "a"), }), "function1"),
          ASTNode::compound_statement({
            ASTNode::assign(ASTNode::identifier("a"), ASTNode::integer("1"))
          })))
      );
}

TEST(IRGenerator, Multiply) {
  EXPECT_EQ(
      "@function:\n"
      "  %0 = 1 * 2;", 
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::multiply(
                ASTNode::integer("1"),
                ASTNode::integer("2"))
            })))
      );
  EXPECT_EQ(
      "@function:\n"
      "  alloca %0 size 4;\n"
      "  alloca %1 size 4;\n"
      "  %2 = %1 * %0;", 
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var2"), nullptr),
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), nullptr),
            ASTNode::multiply(ASTNode::identifier("var1"), ASTNode::identifier("var2"))
            }))
        ));
  EXPECT_EQ(
      "@function:\n"
      "  alloca %0 size 4;\n"
      "  alloca %1 size 4;\n"
      "  %2 = %0 * %1;", 
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var2"), nullptr),
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), nullptr),
            ASTNode::multiply(ASTNode::identifier("var2"), ASTNode::identifier("var1"))
            }))
        ));
}

TEST(IRGenerator, Divide) {
  EXPECT_EQ(
      "@function:\n"
      "  %0 = 1 / 2;", 
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::divide(
                ASTNode::integer("1"),
                ASTNode::integer("2"))
            }))
        ));
  EXPECT_EQ(
      "@function:\n"
      "  %0 = 1 / 2;", 
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::divide(
                ASTNode::integer("1"),
                ASTNode::integer("2"))
            }))
        ));
}

TEST(IRGenerator, Addition) {
  EXPECT_EQ(
      "@function:\n"
      "  %0 = 1 + 2;", 
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::add(
                ASTNode::integer("1"),
                ASTNode::integer("2"))
            }))
        ));
}

TEST(IRGenerator, Subtraction) {
  EXPECT_EQ(
      "@function:\n"
      "  %0 = 1 + 2;", 
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::add(
                ASTNode::integer("1"),
                ASTNode::integer("2"))
            }))
        ));
}

TEST(IRGenerator, Equal) {
  EXPECT_EQ(
      "@function:\n"
      "  %0 = 1 == 2;", 
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::equal(
                ASTNode::integer("1"),
                ASTNode::integer("2"))
            }))
        ));
}

TEST(IRGenerator, NotEqual) {
  EXPECT_EQ(
      "@function:\n"
      "  %0 = 1 != 2;", 
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::not_equal(
                ASTNode::integer("1"),
                ASTNode::integer("2"))
            }))
        ));
}

TEST(IRGenerator, LessThan) {
  EXPECT_EQ(
      "@function:\n"
      "  %0 = 1 < 2;", 
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::less_than(
                ASTNode::integer("1"),
                ASTNode::integer("2"))
            }))
        ));
}

TEST(IRGenerator, LessEqual) {
  EXPECT_EQ(
      "@function:\n"
      "  %0 = 1 <= 2;", 
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::less_equal(
                ASTNode::integer("1"),
                ASTNode::integer("2"))
            }))
        ));
}

TEST(IRGenerator, Negtive) {
  EXPECT_EQ(
      "@function:\n"
      "  %0 = -1;", 
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::negtive(
                ASTNode::integer("1"))
            }))
        ));
}

TEST(IRGenerator, Statements) {
  EXPECT_EQ(
      "@function:\n"
      "  %0 = 1 + 2;\n"
      "  %1 = 1 - 2;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::add(
                ASTNode::integer("1"),
                ASTNode::integer("2")),
            ASTNode::subtract(
                ASTNode::integer("1"),
                ASTNode::integer("2")),
            }))
        ));
}

TEST(IRGenerator, Recursive) {
  EXPECT_EQ(
      "@function:\n"
      "  %0 = 1 * 2;\n"
      "  %1 = -3;\n"
      "  %2 = %0 * %1;", 
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::multiply(
                ASTNode::multiply(
                  ASTNode::integer("1"),
                  ASTNode::integer("2")),
                ASTNode::negtive(
                  ASTNode::integer("3"))
                )
            }))
        ));
}

TEST(IRGenerator, Assign) {
  EXPECT_EQ(
      "@function:\n"
      "  alloca %0 size 4;\n"
      "  alloca %1 size 4;\n"
      "  %1 = 2;\n"
      "  %0 = %1;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), nullptr),
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var2"), nullptr),
            ASTNode::assign(ASTNode::identifier("var1"),
                ASTNode::assign(ASTNode::identifier("var2"), ASTNode::integer("2")))
            }))
        ));
  EXPECT_EQ(
      "@function:\n"
      "  alloca %0 size 4;\n"
      "  alloca %1 size 4;\n"
      "  %2 = %0 + %0;\n"
      "  %1 = %2;\n"
      "  %0 = %1;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), nullptr),
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var2"), nullptr),
            ASTNode::assign(
                ASTNode::identifier("var1"),
                ASTNode::assign(
                  ASTNode::identifier("var2"),
                  ASTNode::add(
                    ASTNode::identifier("var1"), 
                    ASTNode::identifier("var1")))
                )
            }))
        ));
}

TEST(IRGenerator, CompoundStatement) {
  EXPECT_EQ(
      "@function:\n"
      "  alloca %0 size 4;\n"
      "  alloca %1 size 4;\n"
      "  alloca %2 size 4;\n"
      "  %0 = %1;\n"
      "  %2 = 2;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::compound_statement({
                ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), nullptr),
                ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var2"), nullptr),
                ASTNode::assign(ASTNode::identifier("var1"), ASTNode::identifier("var2"))}),
            ASTNode::compound_statement({
                ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), nullptr),
                ASTNode::compound_statement({
                    ASTNode::assign(ASTNode::identifier("var1"), ASTNode::integer("2"))})
                }),
            ASTNode::compound_statement({})
            })
          )
        ));
  EXPECT_EQ(
      "@function:\n"
      "  alloca %0 size 4;\n"
      "  alloca %1 size 4;\n"
      "  alloca %2 size 4;\n"
      "  alloca %3 size 4;\n"
      "  %0 = 2;\n"
      "  %1 = %0;\n"
      "  %2 = 3;\n"
      "  %3 = %2;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::compound_statement({
                ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), ASTNode::integer("2")),
                ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var2"), ASTNode::identifier("var1")),
            }),
            ASTNode::compound_statement({
                ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), ASTNode::integer("3")),
                ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var2"), ASTNode::identifier("var1")),
            }),
            })
          )
        ));
}

TEST(IRGenerator, SelectStatement) {
  EXPECT_EQ(
      "@function:\n"
      "  alloca %0 size 4;\n"
      "  alloca %1 size 4;\n"
      "  if-false %0 goto Label.0;\n"
      "  %0 = 1;\n"
      "  goto Label.1;\n"
      "Label.0:\n"
      "  if-false %1 goto Label.2;\n"
      "  %1 = 2;\n"
      "  goto Label.3;\n"
      "Label.2:\n"
      "  %0 = 3;\n"
      "Label.3:\n"
      "Label.1:\n"
      "  nope;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), nullptr),
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var2"), nullptr),
            ASTNode::if_else(
                ASTNode::identifier("var1"),
                ASTNode::compound_statement({
                  ASTNode::assign(
                      ASTNode::identifier("var1"),
                      ASTNode::integer("1")),
                  }),
                ASTNode::if_else(
                  ASTNode::identifier("var2"), 
                  ASTNode::compound_statement({
                    ASTNode::assign(
                        ASTNode::identifier("var2"),
                        ASTNode::integer("2"))
                    }),
                  ASTNode::compound_statement({
                    ASTNode::assign(
                        ASTNode::identifier("var1"),
                        ASTNode::integer("3"))
                    })
                  )
                )
          })
      )
    ));
  EXPECT_EQ(
      "@function:\n"
      "  alloca %0 size 4;\n"
      "  alloca %1 size 4;\n"
      "  alloca %2 size 4;\n"
      "  alloca %3 size 4;\n"
      "  if-false %0 goto Label.0;\n"
      "  %0 = 1;\n"
      "  goto Label.1;\n"
      "Label.0:\n"
      "  if-false %1 goto Label.2;\n"
      "  %1 = 2;\n"
      "  goto Label.3;\n"
      "Label.2:\n"
      "  %0 = 3;\n"
      "Label.3:\n"
      "Label.1:\n"
      "  %2 = %3;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), nullptr),
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var2"), nullptr),
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var3"), nullptr),
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var4"), nullptr),
            ASTNode::if_else(
                ASTNode::identifier("var1"),
                ASTNode::compound_statement({
                  ASTNode::assign(
                      ASTNode::identifier("var1"),
                      ASTNode::integer("1")),
                  }),
                ASTNode::if_else(
                  ASTNode::identifier("var2"), 
                  ASTNode::compound_statement({
                    ASTNode::assign(
                        ASTNode::identifier("var2"),
                        ASTNode::integer("2"))
                    }),
                  ASTNode::compound_statement({
                    ASTNode::assign(
                        ASTNode::identifier("var1"),
                        ASTNode::integer("3"))
                    })
                  )
                ),
                ASTNode::assign(
                    ASTNode::identifier("var3"),
                    ASTNode::identifier("var4"))
          })
  )
    ));
}

TEST(IRGenerator, IterationStatement) {
  EXPECT_EQ(
      "@function:\n"
      "Label.0:\n"
      "  goto Label.0;\n" 
      "Label.1:\n"
      "  nope;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::for_loop(
                ASTNode::empty(),
                ASTNode::empty(),
                ASTNode::empty(),
                ASTNode::compound_statement({ }))
            })
          )
        ));
  EXPECT_EQ(
      "@function:\n"
      "  alloca %0 size 4;\n"
      "  alloca %1 size 4;\n"
      "  alloca %2 size 4;\n"
      "  %0 = 1;\n"
      "Label.0:\n"
      "  if-false %1 goto Label.1;\n"
      "  %0 = 3;\n" 
      "  goto Label.0;\n"
      "Label.1:\n"
      "  nope;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), nullptr),
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var2"), nullptr),
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var3"), nullptr),
            ASTNode::for_loop(
                ASTNode::assign(
                  ASTNode::identifier("var1"),
                  ASTNode::integer("1")),
                ASTNode::identifier("var2"),
                ASTNode::identifier("var3"),
                ASTNode::compound_statement({
                  ASTNode::assign(ASTNode::identifier("var1"), ASTNode::integer("3"))
                  }))
            })
          )
        ));
  EXPECT_EQ(
      "@function:\n"
      "  alloca %0 size 4;\n"
      "  alloca %1 size 4;\n"
      "  alloca %2 size 4;\n"
      "  %0 = 1;\n"
      "Label.0:\n"
      "  if-false %1 goto Label.1;\n"
      "  %0 = 3;\n" 
      "  %2 = 1;\n"
      "  goto Label.0;\n"
      "Label.1:\n"
      "  nope;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), nullptr),
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var2"), nullptr),
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var3"), nullptr),
            ASTNode::for_loop(
                ASTNode::assign(
                  ASTNode::identifier("var1"),
                  ASTNode::integer("1")),
                ASTNode::identifier("var2"),
                ASTNode::assign(
                  ASTNode::identifier("var3"),
                  ASTNode::integer("1")),
                ASTNode::compound_statement({
                  ASTNode::assign(ASTNode::identifier("var1"), ASTNode::integer("3"))
                  }))
            })
          )
        ));
  EXPECT_EQ(
      "@function:\n"
      "  alloca %0 size 4;\n"
      "  alloca %1 size 4;\n"
      "  alloca %2 size 4;\n"
      "  %0 = 1;\n"
      "Label.0:\n"
      "  %0 = 3;\n" 
      "  %2 = 1;\n"
      "  goto Label.0;\n"
      "Label.1:\n"
      "  nope;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), nullptr),
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var2"), nullptr),
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var3"), nullptr),
            ASTNode::for_loop(
                ASTNode::assign(
                  ASTNode::identifier("var1"),
                  ASTNode::integer("1")),
                ASTNode::empty(),
                ASTNode::assign(
                  ASTNode::identifier("var3"),
                  ASTNode::integer("1")),
                ASTNode::compound_statement({
                  ASTNode::assign(ASTNode::identifier("var1"), ASTNode::integer("3"))
                  }))
            })
          )
        ));

  EXPECT_EQ(
      "@function:\n"
      "  alloca %0 size 4;\n"
      "Label.0:\n"
      "  if-false %0 goto Label.1;\n"
      "  goto Label.0;\n"
      "Label.1:\n"
      "  nope;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), nullptr),
            ASTNode::while_loop(
                ASTNode::identifier("var1"),
                ASTNode::empty())
            }))
        ));
  EXPECT_EQ(
      "@function:\n"
      "  alloca %0 size 4;\n"
      "Label.0:\n"
      "  if-false %0 goto Label.1;\n"
      "  %0 = 3;\n"
      "  goto Label.0;\n"
      "Label.1:\n"
      "  nope;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), nullptr),
            ASTNode::while_loop(
                ASTNode::identifier("var1"),
                ASTNode::assign(ASTNode::identifier("var1"), ASTNode::integer("3")))
            }))
        ));
  EXPECT_EQ(
      "@function:\n"
      "  alloca %0 size 4;\n"
      "Label.0:\n"
      "  %0 = 3;\n"
      "  if %0 goto Label.0;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), nullptr),
            ASTNode::do_while(
                ASTNode::assign(ASTNode::identifier("var1"), ASTNode::integer("3")),
                ASTNode::identifier("var1"))
            }))
        ));
  EXPECT_EQ(
      "@function:\n"
      "  alloca %0 size 4;\n"
      "Label.0:\n"
      "  %0 = 3;\n"
      "  if %0 goto Label.0;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "var1"), nullptr),
            ASTNode::do_while(
                ASTNode::empty(),
                ASTNode::assign(ASTNode::identifier("var1"), ASTNode::integer("3")))
            }))
        ));

}

TEST(IRGenerator, Declaration) {
  EXPECT_EQ(
      "@function:\n"
      "  alloca %0 size 4;\n"
      "  alloca %1 size 4;\n"
      "  alloca %2 size 8;\n"
      "  %3 = %1 + 10;\n"
      "  %0 = %3;\n"
      "  %2 = %0;",
      IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::declaration_statement({
                ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "c"), nullptr),
                ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "b"), nullptr),
                ASTNode::declaration(
                    Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)), "a"),
                    ASTNode::assign(ASTNode::identifier("c"), ASTNode::add(ASTNode::identifier("b"), ASTNode::integer("10")))
                    ),
                })
            })
          )
        ));
  EXPECT_THROW(IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::declaration_statement({
                ASTNode::declaration(Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)), "a"), nullptr),
                ASTNode::declaration(Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)), "a"), nullptr),
                })
            })
          )),
      std::invalid_argument);
  EXPECT_THROW(IRStringGenerate(
        ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "function"),
          ASTNode::compound_statement({
            ASTNode::declaration_statement({
                ASTNode::declaration(Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)), "a"), nullptr),
                ASTNode::declaration(Declarator::Create(Type::basic(TypeKind::INT), "a"), nullptr),
                })
            })
          )),
      std::invalid_argument);
}
