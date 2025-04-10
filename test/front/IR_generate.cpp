#include "front/IR_generator.h"
#include "gtest/gtest.h"
#include <sstream>

namespace front {

static std::string IRStringGenerate(const ASTNodePtr root) {
  auto generator = CreateIRGenerator(std::move(root));
  auto IR_list = generator->work();
  std::stringstream result_builder;
  SiiIR::IDAllocator id_allocator;
  for (size_t i = 0; i < IR_list->size(); i++) {
    result_builder << (*IR_list)[i]->to_string(id_allocator);
    if (i != IR_list->size() - 1) {
      result_builder << "\n";
    }
  }
  return result_builder.str();
}

TEST(IRGenerator, FunctionDeclaration) {
  EXPECT_EQ(
      "@function1():\n",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function1"),
          ASTNode::Compound_statement({}))));
  EXPECT_EQ(
      "@function1():\n"
      "  %0 = alloca size 4;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function1"),
          ASTNode::Compound_statement({ASTNode::Declaration(
              Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
              nullptr)}))));
  EXPECT_EQ(
      "@function1():\n"
      "  %0 = alloca size 4;\n"
      "  store 1 to %0;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function1"),
          ASTNode::Compound_statement({ASTNode::Declaration(
              Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
              ASTNode::Integer("1"))}))));
  EXPECT_EQ(
      "@function1():\n"
      "  %0 = alloca size 4;\n"
      "  %1 = alloca size 4;\n"
      "  store 0 to %0;\n"
      "  store 1 to %1;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function1"),
          ASTNode::Compound_statement({
              ASTNode::Declaration(
                  Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                  ASTNode::Integer("0")),
              ASTNode::Declaration(
                  Declarator::Create(Type::Basic(TypeKind::INT), "var2"),
                  ASTNode::Integer("1")),
          }))));
  EXPECT_EQ(
      "@function1():\n"
      "  %0 = alloca size 4;\n"
      "  %1 = alloca size 4;\n"
      "  store 0 to %0;\n"
      "  store 1 to %1;\n"
      "  %2 = load %1;\n"
      "  store %2 to %0;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function1"),
          ASTNode::Compound_statement(
              {ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                   ASTNode::Integer("0")),
               ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var2"),
                   ASTNode::Integer("1")),
               ASTNode::Assign(ASTNode::Identifier("var1"),
                               ASTNode::Identifier("var2"))}))));
  EXPECT_EQ(
      "@function1():\n"
      "  %0 = alloca size 4;\n"
      "  %1 = alloca size 4;\n"
      "  store 0 to %0;\n"
      "  store 1 to %1;\n"
      "  %2 = load %1;\n"
      "  %3 = 1 + 2;\n"
      "  %4 = %2 + %3;\n"
      "  store %4 to %0;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function1"),
          ASTNode::Compound_statement(
              {ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                   ASTNode::Integer("0")),
               ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var2"),
                   ASTNode::Integer("1")),
               ASTNode::Assign(
                   ASTNode::Identifier("var1"),
                   ASTNode::Add(ASTNode::Identifier("var2"),
                                ASTNode::Add(ASTNode::Integer("1"),
                                             ASTNode::Integer("2"))))}))));
  EXPECT_EQ(
      "@function1():\n"
      "  %0 = alloca size 4;\n"
      "  %1 = alloca size 4;\n"
      "  store 0 to %0;\n"
      "  %2 = 1 + 2;\n"
      "  store %2 to %0;\n"
      "  store 1 to %1;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function1"),
          ASTNode::Compound_statement(
              {ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                   ASTNode::Integer("0")),
               ASTNode::Assign(
                   ASTNode::Identifier("var1"),
                   ASTNode::Add(ASTNode::Integer("1"), ASTNode::Integer("2"))),
               ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var2"),
                   ASTNode::Integer("1"))}))));
}

TEST(IRGenerator, FunctionDeclarationWithParameter) {
  EXPECT_EQ("@function1(%0):\n"
            "  %1 = alloca size 4;\n"
            "  store %0 to %1;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(
                        Type::Basic(TypeKind::INT),
                        {
                            Declarator::Create(Type::Basic(TypeKind::INT), "a"),
                        }),
                    "function1"),
                ASTNode::Compound_statement({}))));
  EXPECT_THROW(
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(
              Type::Function(
                  Type::Basic(TypeKind::INT),
                  {
                      Declarator::Create(Type::Basic(TypeKind::INT), "a"),
                      Declarator::Create(Type::Basic(TypeKind::INT), "a"),
                  }),
              "function1"),
          ASTNode::Compound_statement({}))),
      std::invalid_argument);
  EXPECT_EQ("@function1(%0):\n"
            "  %1 = alloca size 4;\n"
            "  store %0 to %1;\n"
            "  store 1 to %1;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(
                        Type::Basic(TypeKind::INT),
                        {
                            Declarator::Create(Type::Basic(TypeKind::INT), "a"),
                        }),
                    "function1"),
                ASTNode::Compound_statement({ASTNode::Assign(
                    ASTNode::Identifier("a"), ASTNode::Integer("1"))}))));
}

TEST(IRGenerator, Multiply) {
  EXPECT_EQ("@function():\n"
            "  %0 = 1 * 2;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(Type::Basic(TypeKind::INT), {}), "function"),
                ASTNode::Compound_statement({ASTNode::Multiply(
                    ASTNode::Integer("1"), ASTNode::Integer("2"))}))));
  EXPECT_EQ("@function():\n"
            "  %0 = alloca size 4;\n"
            "  %1 = alloca size 4;\n"
            "  %2 = load %1;\n"
            "  %3 = load %0;\n"
            "  %4 = %2 * %3;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(Type::Basic(TypeKind::INT), {}), "function"),
                ASTNode::Compound_statement(
                    {ASTNode::Declaration(
                         Declarator::Create(Type::Basic(TypeKind::INT), "var2"),
                         nullptr),
                     ASTNode::Declaration(
                         Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                         nullptr),
                     ASTNode::Multiply(ASTNode::Identifier("var1"),
                                       ASTNode::Identifier("var2"))}))));
  EXPECT_EQ("@function():\n"
            "  %0 = alloca size 4;\n"
            "  %1 = alloca size 4;\n"
            "  %2 = load %0;\n"
            "  %3 = load %1;\n"
            "  %4 = %2 * %3;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(Type::Basic(TypeKind::INT), {}), "function"),
                ASTNode::Compound_statement(
                    {ASTNode::Declaration(
                         Declarator::Create(Type::Basic(TypeKind::INT), "var2"),
                         nullptr),
                     ASTNode::Declaration(
                         Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                         nullptr),
                     ASTNode::Multiply(ASTNode::Identifier("var2"),
                                       ASTNode::Identifier("var1"))}))));
}

TEST(IRGenerator, Divide) {
  EXPECT_EQ("@function():\n"
            "  %0 = 1 / 2;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(Type::Basic(TypeKind::INT), {}), "function"),
                ASTNode::Compound_statement({ASTNode::Divide(
                    ASTNode::Integer("1"), ASTNode::Integer("2"))}))));
  EXPECT_EQ("@function():\n"
            "  %0 = 1 / 2;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(Type::Basic(TypeKind::INT), {}), "function"),
                ASTNode::Compound_statement({ASTNode::Divide(
                    ASTNode::Integer("1"), ASTNode::Integer("2"))}))));
}

TEST(IRGenerator, Addition) {
  EXPECT_EQ("@function():\n"
            "  %0 = 1 + 2;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(Type::Basic(TypeKind::INT), {}), "function"),
                ASTNode::Compound_statement({ASTNode::Add(
                    ASTNode::Integer("1"), ASTNode::Integer("2"))}))));
}

TEST(IRGenerator, Subtraction) {
  EXPECT_EQ("@function():\n"
            "  %0 = 1 + 2;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(Type::Basic(TypeKind::INT), {}), "function"),
                ASTNode::Compound_statement({ASTNode::Add(
                    ASTNode::Integer("1"), ASTNode::Integer("2"))}))));
}

TEST(IRGenerator, Equal) {
  EXPECT_EQ("@function():\n"
            "  %0 = 1 == 2;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(Type::Basic(TypeKind::INT), {}), "function"),
                ASTNode::Compound_statement({ASTNode::Equal(
                    ASTNode::Integer("1"), ASTNode::Integer("2"))}))));
}

TEST(IRGenerator, NotEqual) {
  EXPECT_EQ("@function():\n"
            "  %0 = 1 != 2;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(Type::Basic(TypeKind::INT), {}), "function"),
                ASTNode::Compound_statement({ASTNode::Not_equal(
                    ASTNode::Integer("1"), ASTNode::Integer("2"))}))));
}

TEST(IRGenerator, LessThan) {
  EXPECT_EQ("@function():\n"
            "  %0 = 1 < 2;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(Type::Basic(TypeKind::INT), {}), "function"),
                ASTNode::Compound_statement({ASTNode::Less_than(
                    ASTNode::Integer("1"), ASTNode::Integer("2"))}))));
}

TEST(IRGenerator, LessEqual) {
  EXPECT_EQ("@function():\n"
            "  %0 = 1 <= 2;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(Type::Basic(TypeKind::INT), {}), "function"),
                ASTNode::Compound_statement({ASTNode::Less_equal(
                    ASTNode::Integer("1"), ASTNode::Integer("2"))}))));
}

TEST(IRGenerator, Negtive) {
  EXPECT_EQ("@function():\n"
            "  %0 = -1;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(Type::Basic(TypeKind::INT), {}), "function"),
                ASTNode::Compound_statement(
                    {ASTNode::Negtive(ASTNode::Integer("1"))}))));
}

TEST(IRGenerator, Statements) {
  EXPECT_EQ(
      "@function():\n"
      "  %0 = 1 + 2;\n"
      "  %1 = 1 - 2;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement({
              ASTNode::Add(ASTNode::Integer("1"), ASTNode::Integer("2")),
              ASTNode::Subtract(ASTNode::Integer("1"), ASTNode::Integer("2")),
          }))));
}

TEST(IRGenerator, Recursive) {
  EXPECT_EQ(
      "@function():\n"
      "  %0 = 1 * 2;\n"
      "  %1 = -3;\n"
      "  %2 = %0 * %1;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement({ASTNode::Multiply(
              ASTNode::Multiply(ASTNode::Integer("1"), ASTNode::Integer("2")),
              ASTNode::Negtive(ASTNode::Integer("3")))}))));
}

TEST(IRGenerator, Assign) {
  EXPECT_EQ(
      "@function():\n"
      "  %0 = alloca size 4;\n"
      "  %1 = alloca size 4;\n"
      "  store 2 to %1;\n"
      "  store 2 to %0;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement(
              {ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                   nullptr),
               ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var2"),
                   nullptr),
               ASTNode::Assign(ASTNode::Identifier("var1"),
                               ASTNode::Assign(ASTNode::Identifier("var2"),
                                               ASTNode::Integer("2")))}))));
  EXPECT_EQ("@function():\n"
            "  %0 = alloca size 4;\n"
            "  %1 = alloca size 4;\n"
            "  %2 = load %0;\n"
            "  %3 = load %0;\n"
            "  %4 = %2 + %3;\n"
            "  store %4 to %1;\n"
            "  store %4 to %0;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(Type::Basic(TypeKind::INT), {}), "function"),
                ASTNode::Compound_statement(
                    {ASTNode::Declaration(
                         Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                         nullptr),
                     ASTNode::Declaration(
                         Declarator::Create(Type::Basic(TypeKind::INT), "var2"),
                         nullptr),
                     ASTNode::Assign(
                         ASTNode::Identifier("var1"),
                         ASTNode::Assign(
                             ASTNode::Identifier("var2"),
                             ASTNode::Add(ASTNode::Identifier("var1"),
                                          ASTNode::Identifier("var1"))))}))));
}

TEST(IRGenerator, CompoundStatement) {
  EXPECT_EQ(
      "@function():\n"
      "  %0 = alloca size 4;\n"
      "  %1 = alloca size 4;\n"
      "  %2 = alloca size 4;\n"
      "  %3 = load %1;\n"
      "  store %3 to %0;\n"
      "  store 2 to %2;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement(
              {ASTNode::Compound_statement(
                   {ASTNode::Declaration(
                        Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                        nullptr),
                    ASTNode::Declaration(
                        Declarator::Create(Type::Basic(TypeKind::INT), "var2"),
                        nullptr),
                    ASTNode::Assign(ASTNode::Identifier("var1"),
                                    ASTNode::Identifier("var2"))}),
               ASTNode::Compound_statement(
                   {ASTNode::Declaration(
                        Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                        nullptr),
                    ASTNode::Compound_statement({ASTNode::Assign(
                        ASTNode::Identifier("var1"), ASTNode::Integer("2"))})}),
               ASTNode::Compound_statement({})}))));
  EXPECT_EQ(
      "@function():\n"
      "  %0 = alloca size 4;\n"
      "  %1 = alloca size 4;\n"
      "  %2 = alloca size 4;\n"
      "  %3 = alloca size 4;\n"
      "  store 2 to %0;\n"
      "  %4 = load %0;\n"
      "  store %4 to %1;\n"
      "  store 3 to %2;\n"
      "  %5 = load %2;\n"
      "  store %5 to %3;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement({
              ASTNode::Compound_statement({
                  ASTNode::Declaration(
                      Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                      ASTNode::Integer("2")),
                  ASTNode::Declaration(
                      Declarator::Create(Type::Basic(TypeKind::INT), "var2"),
                      ASTNode::Identifier("var1")),
              }),
              ASTNode::Compound_statement({
                  ASTNode::Declaration(
                      Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                      ASTNode::Integer("3")),
                  ASTNode::Declaration(
                      Declarator::Create(Type::Basic(TypeKind::INT), "var2"),
                      ASTNode::Identifier("var1")),
              }),
          }))));
}

TEST(IRGenerator, SelectStatement) {
  EXPECT_EQ("@function():\n"
            "  %0 = alloca size 4;\n"
            "  %1 = alloca size 4;\n"
            "  %2 = load %0;\n"
            "  %3 = %2 != 0;\n"
            "  if %3 goto Label.4 else Label.5;\n"
            "Label.4:\n"
            "  store 1 to %0;\n"
            "  goto Label.6;\n"
            "Label.5:\n"
            "  %7 = load %1;\n"
            "  %8 = %7 != 0;\n"
            "  if %8 goto Label.9 else Label.10;\n"
            "Label.9:\n"
            "  store 2 to %1;\n"
            "  goto Label.11;\n"
            "Label.10:\n"
            "  store 3 to %0;\n"
            "  goto Label.11;\n"
            "Label.11:\n"
            "  goto Label.6;\n"
            "Label.6:\n"
            "  nope;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(Type::Basic(TypeKind::INT), {}), "function"),
                ASTNode::Compound_statement(
                    {ASTNode::Declaration(
                         Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                         nullptr),
                     ASTNode::Declaration(
                         Declarator::Create(Type::Basic(TypeKind::INT), "var2"),
                         nullptr),
                     ASTNode::If_else(
                         ASTNode::Identifier("var1"),
                         ASTNode::Compound_statement({
                             ASTNode::Assign(ASTNode::Identifier("var1"),
                                             ASTNode::Integer("1")),
                         }),
                         ASTNode::If_else(
                             ASTNode::Identifier("var2"),
                             ASTNode::Compound_statement(
                                 {ASTNode::Assign(ASTNode::Identifier("var2"),
                                                  ASTNode::Integer("2"))}),
                             ASTNode::Compound_statement({ASTNode::Assign(
                                 ASTNode::Identifier("var1"),
                                 ASTNode::Integer("3"))})))}))));
  EXPECT_EQ("@function():\n"
            "  %0 = alloca size 4;\n"
            "  %1 = alloca size 4;\n"
            "  %2 = alloca size 4;\n"
            "  %3 = alloca size 4;\n"
            "  %4 = load %0;\n"
            "  %5 = %4 != 0;\n"
            "  if %5 goto Label.6 else Label.7;\n"
            "Label.6:\n"
            "  store 1 to %0;\n"
            "  goto Label.8;\n"
            "Label.7:\n"
            "  %9 = load %1;\n"
            "  %10 = %9 != 0;\n"
            "  if %10 goto Label.11 else Label.12;\n"
            "Label.11:\n"
            "  store 2 to %1;\n"
            "  goto Label.13;\n"
            "Label.12:\n"
            "  store 3 to %0;\n"
            "  goto Label.13;\n"
            "Label.13:\n"
            "  goto Label.8;\n"
            "Label.8:\n"
            "  %14 = load %3;\n"
            "  store %14 to %2;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(Type::Basic(TypeKind::INT), {}), "function"),
                ASTNode::Compound_statement(
                    {ASTNode::Declaration(
                         Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                         nullptr),
                     ASTNode::Declaration(
                         Declarator::Create(Type::Basic(TypeKind::INT), "var2"),
                         nullptr),
                     ASTNode::Declaration(
                         Declarator::Create(Type::Basic(TypeKind::INT), "var3"),
                         nullptr),
                     ASTNode::Declaration(
                         Declarator::Create(Type::Basic(TypeKind::INT), "var4"),
                         nullptr),
                     ASTNode::If_else(
                         ASTNode::Identifier("var1"),
                         ASTNode::Compound_statement({
                             ASTNode::Assign(ASTNode::Identifier("var1"),
                                             ASTNode::Integer("1")),
                         }),
                         ASTNode::If_else(
                             ASTNode::Identifier("var2"),
                             ASTNode::Compound_statement(
                                 {ASTNode::Assign(ASTNode::Identifier("var2"),
                                                  ASTNode::Integer("2"))}),
                             ASTNode::Compound_statement(
                                 {ASTNode::Assign(ASTNode::Identifier("var1"),
                                                  ASTNode::Integer("3"))}))),
                     ASTNode::Assign(ASTNode::Identifier("var3"),
                                     ASTNode::Identifier("var4"))}))));
}

TEST(IRGenerator, IterationStatement) {
  EXPECT_EQ("@function():\n"
            "Label.0:\n"
            "  goto Label.0;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(Type::Basic(TypeKind::INT), {}), "function"),
                ASTNode::Compound_statement({ASTNode::For_loop(
                    ASTNode::empty(), ASTNode::empty(), ASTNode::empty(),
                    ASTNode::Compound_statement({}))}))));
  EXPECT_EQ(
      "@function():\n"
      "  %0 = alloca size 4;\n"
      "  %1 = alloca size 4;\n"
      "  %2 = alloca size 4;\n"
      "  store 1 to %0;\n"
      "Label.3:\n"
      "  %4 = load %1;\n"
      "  %5 = %4 != 0;\n"
      "  if %5 goto Label.6 else Label.7;\n"
      "Label.6:\n"
      "  store 3 to %0;\n"
      "  goto Label.3;\n"
      "Label.7:\n"
      "  nope;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement(
              {ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                   nullptr),
               ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var2"),
                   nullptr),
               ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var3"),
                   nullptr),
               ASTNode::For_loop(
                   ASTNode::Assign(ASTNode::Identifier("var1"),
                                   ASTNode::Integer("1")),
                   ASTNode::Identifier("var2"), ASTNode::Identifier("var3"),
                   ASTNode::Compound_statement(
                       {ASTNode::Assign(ASTNode::Identifier("var1"),
                                        ASTNode::Integer("3"))}))}))));
  EXPECT_EQ(
      "@function():\n"
      "  %0 = alloca size 4;\n"
      "  %1 = alloca size 4;\n"
      "  %2 = alloca size 4;\n"
      "  store 1 to %0;\n"
      "Label.3:\n"
      "  %4 = load %1;\n"
      "  %5 = %4 != 0;\n"
      "  if %5 goto Label.6 else Label.7;\n"
      "Label.6:\n"
      "  store 3 to %0;\n"
      "  store 1 to %2;\n"
      "  goto Label.3;\n"
      "Label.7:\n"
      "  nope;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement(
              {ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                   nullptr),
               ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var2"),
                   nullptr),
               ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var3"),
                   nullptr),
               ASTNode::For_loop(ASTNode::Assign(ASTNode::Identifier("var1"),
                                                 ASTNode::Integer("1")),
                                 ASTNode::Identifier("var2"),
                                 ASTNode::Assign(ASTNode::Identifier("var3"),
                                                 ASTNode::Integer("1")),
                                 ASTNode::Compound_statement({ASTNode::Assign(
                                     ASTNode::Identifier("var1"),
                                     ASTNode::Integer("3"))}))}))));
  EXPECT_EQ(
      "@function():\n"
      "  %0 = alloca size 4;\n"
      "  %1 = alloca size 4;\n"
      "  %2 = alloca size 4;\n"
      "  store 1 to %0;\n"
      "Label.3:\n"
      "  store 3 to %0;\n"
      "  store 1 to %2;\n"
      "  goto Label.3;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement(
              {ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                   nullptr),
               ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var2"),
                   nullptr),
               ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var3"),
                   nullptr),
               ASTNode::For_loop(ASTNode::Assign(ASTNode::Identifier("var1"),
                                                 ASTNode::Integer("1")),
                                 ASTNode::empty(),
                                 ASTNode::Assign(ASTNode::Identifier("var3"),
                                                 ASTNode::Integer("1")),
                                 ASTNode::Compound_statement({ASTNode::Assign(
                                     ASTNode::Identifier("var1"),
                                     ASTNode::Integer("3"))}))}))));

  EXPECT_EQ("@function():\n"
            "  %0 = alloca size 4;\n"
            "Label.1:\n"
            "  %2 = load %0;\n"
            "  %3 = %2 != 0;\n"
            "  if %3 goto Label.4 else Label.5;\n"
            "Label.4:\n"
            "  goto Label.1;\n"
            "Label.5:\n"
            "  nope;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(Type::Basic(TypeKind::INT), {}), "function"),
                ASTNode::Compound_statement(
                    {ASTNode::Declaration(
                         Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                         nullptr),
                     ASTNode::While_loop(ASTNode::Identifier("var1"),
                                         ASTNode::empty())}))));
  EXPECT_EQ(
      "@function():\n"
      "  %0 = alloca size 4;\n"
      "Label.1:\n"
      "  %2 = load %0;\n"
      "  %3 = %2 != 0;\n"
      "  if %3 goto Label.4 else Label.5;\n"
      "Label.4:\n"
      "  store 3 to %0;\n"
      "  goto Label.1;\n"
      "Label.5:\n"
      "  nope;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement(
              {ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                   nullptr),
               ASTNode::While_loop(ASTNode::Identifier("var1"),
                                   ASTNode::Assign(ASTNode::Identifier("var1"),
                                                   ASTNode::Integer("3")))}))));
  EXPECT_EQ(
      "@function():\n"
      "  %0 = alloca size 4;\n"
      "Label.1:\n"
      "  store 3 to %0;\n"
      "  %2 = load %0;\n"
      "  %3 = %2 != 0;\n"
      "  if %3 goto Label.1 else Label.4;\n"
      "Label.4:\n"
      "  nope;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement(
              {ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                   nullptr),
               ASTNode::Do_while(ASTNode::Assign(ASTNode::Identifier("var1"),
                                                 ASTNode::Integer("3")),
                                 ASTNode::Identifier("var1"))}))));
  EXPECT_EQ(
      "@function():\n"
      "  %0 = alloca size 4;\n"
      "Label.1:\n"
      "  store 3 to %0;\n"
      "  %2 = 3 != 0;\n"
      "  if %2 goto Label.1 else Label.3;\n"
      "Label.3:\n"
      "  nope;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement(
              {ASTNode::Declaration(
                   Declarator::Create(Type::Basic(TypeKind::INT), "var1"),
                   nullptr),
               ASTNode::Do_while(ASTNode::empty(),
                                 ASTNode::Assign(ASTNode::Identifier("var1"),
                                                 ASTNode::Integer("3")))}))));
}

TEST(IRGenerator, Declaration) {
  EXPECT_EQ(
      "@function():\n"
      "  %0 = alloca size 4;\n"
      "  %1 = alloca size 4;\n"
      "  %2 = alloca size 4;\n"
      "  %3 = load %1;\n"
      "  %4 = %3 + 10;\n"
      "  store %4 to %0;\n"
      "  store %4 to %2;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement({ASTNode::Declaration_statement({
              ASTNode::Declaration(
                  Declarator::Create(Type::Basic(TypeKind::INT), "c"), nullptr),
              ASTNode::Declaration(
                  Declarator::Create(Type::Basic(TypeKind::INT), "b"), nullptr),
              ASTNode::Declaration(
                  Declarator::Create(Type::Basic(TypeKind::INT), "a"),
                  ASTNode::Assign(ASTNode::Identifier("c"),
                                  ASTNode::Add(ASTNode::Identifier("b"),
                                               ASTNode::Integer("10")))),
          })}))));
  EXPECT_THROW(
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement({ASTNode::Declaration_statement({
              ASTNode::Declaration(
                  Declarator::Create(Type::Pointer(Type::Basic(TypeKind::INT)),
                                     "a"),
                  nullptr),
              ASTNode::Declaration(
                  Declarator::Create(Type::Pointer(Type::Basic(TypeKind::INT)),
                                     "a"),
                  nullptr),
          })}))),
      std::invalid_argument);
  EXPECT_THROW(
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement({ASTNode::Declaration_statement({
              ASTNode::Declaration(
                  Declarator::Create(Type::Pointer(Type::Basic(TypeKind::INT)),
                                     "a"),
                  nullptr),
              ASTNode::Declaration(
                  Declarator::Create(Type::Basic(TypeKind::INT), "a"), nullptr),
          })}))),
      std::invalid_argument);
}

TEST(IRGenerator, GetValue) {
  EXPECT_EQ(
      "@function():\n"
      "  %0 = alloca size 4;\n"
      "  %1 = alloca size 8;\n"
      "  store %0 to %1;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement(
              {ASTNode::Declaration_statement({
                   ASTNode::Declaration(
                       Declarator::Create(Type::Basic(TypeKind::INT), "a"),
                       nullptr),
               }),
               ASTNode::Declaration_statement({
                   ASTNode::Declaration(
                       Declarator::Create(
                           Type::Pointer(Type::Basic(TypeKind::INT)), "b"),
                       ASTNode::Get_address(ASTNode::Identifier("a"))),
               })}))));
  EXPECT_EQ(
      "@function():\n"
      "  %0 = alloca size 4;\n"
      "  %1 = alloca size 8;\n"
      "  %2 = alloca size 8;\n"
      "  store %0 to %1;\n"
      "  store %1 to %2;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement(
              {ASTNode::Declaration_statement({
                   ASTNode::Declaration(
                       Declarator::Create(Type::Basic(TypeKind::INT), "a"),
                       nullptr),
               }),
               ASTNode::Declaration_statement({ASTNode::Declaration(
                   Declarator::Create(Type::Pointer(Type::Basic(TypeKind::INT)),
                                      "b"),
                   ASTNode::Get_address(ASTNode::Identifier("a")))}),
               ASTNode::Declaration_statement({ASTNode::Declaration(
                   Declarator::Create(
                       Type::Pointer(Type::Pointer(Type::Basic(TypeKind::INT))),
                       "c"),
                   ASTNode::Get_address(ASTNode::Identifier("b")))})}))));
}

TEST(IRGenerator, Return) {
  EXPECT_EQ(
      "@function():\n"
      "  %0 = alloca size 4;\n"
      "  %1 = load %0;\n"
      "  return %1;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement(
              {ASTNode::Declaration_statement({
                   ASTNode::Declaration(
                       Declarator::Create(Type::Basic(TypeKind::INT), "a"),
                       nullptr),
               }),
               ASTNode::Return(ASTNode::Identifier("a"))}))));

  EXPECT_EQ(
      "@function():\n"
      "  %0 = alloca size 4;\n"
      "  %1 = load %0;\n"
      "  %2 = %1 + 8;\n"
      "  return %2;",
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement(
              {ASTNode::Declaration_statement({
                   ASTNode::Declaration(
                       Declarator::Create(Type::Basic(TypeKind::INT), "a"),
                       nullptr),
               }),
               ASTNode::Return(ASTNode::Add(ASTNode::Identifier("a"),
                                            ASTNode::Integer("8")))}))));

  EXPECT_EQ("@function():\n"
            "  return 8;",
            IRStringGenerate(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::Function(Type::Basic(TypeKind::INT), {}), "function"),
                ASTNode::Compound_statement(
                    {ASTNode::Return(ASTNode::Integer("8"))}))));

  EXPECT_THROW(
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement({ASTNode::Return(ASTNode::Less_equal(
              ASTNode::Integer("8"), ASTNode::Integer("9")))}))),
      std::runtime_error);
  EXPECT_THROW(
      IRStringGenerate(ASTNode::Function_declaration(
          Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                             "function"),
          ASTNode::Compound_statement({ASTNode::Return(ASTNode::empty())}))),
      std::runtime_error);
}

} // namespace front