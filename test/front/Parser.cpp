#include "front/parser.h"
#include "front/ASTPrinter.h"
#include "gtest/gtest.h"

namespace front {

static auto CreateParser(const std::string &str) {
  std::stringstream ss(str);
  return CreateParser("file_name", ss);
}

static std::string ASTToString(const ASTNodePtr &node) {
  std::stringstream ss;
  ASTPrintVisitor visitor(ss);
  node->accept(visitor);
  return ss.str();
}

TEST(Parser, ArithmeticPrimary) {
  std::string case1 = "{1;}";
  EXPECT_EQ(ASTToString(CreateParser(case1)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({ASTNode::Integer("1")})));
  std::string case2 = "{1; 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case2)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement(
                {ASTNode::Integer("1"), ASTNode::Integer("2")})));
  std::string case3 = "{var1; var2;}";
  EXPECT_EQ(ASTToString(CreateParser(case3)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement(
                {ASTNode::Identifier("var1"), ASTNode::Identifier("var2")})));
  std::string case4 = "{(var1);}";
  EXPECT_EQ(
      ASTToString(CreateParser(case4)->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::Identifier("var1")})));
}

TEST(Parser, ArithmeticUnary) {
  std::string case1 = "{-1;}";
  EXPECT_EQ(ASTToString(CreateParser(case1)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement(
                {ASTNode::Negtive(ASTNode::Integer("1"))})));
  std::string case2 = "{-1; -2;}";
  EXPECT_EQ(ASTToString(CreateParser(case2)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement(
                {ASTNode::Negtive(ASTNode::Integer("1")),
                 ASTNode::Negtive(ASTNode::Integer("2"))})));
  std::string case3 = "{-var1; -var2;}";
  EXPECT_EQ(ASTToString(CreateParser(case3)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement(
                {ASTNode::Negtive(ASTNode::Identifier("var1")),
                 ASTNode::Negtive(ASTNode::Identifier("var2"))})));
  std::string case4 = "{(-var1);}";
  EXPECT_EQ(ASTToString(CreateParser(case4)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({
                ASTNode::Negtive(ASTNode::Identifier("var1")),
            })));
  std::string case5 = "{-(-var1);}";
  EXPECT_EQ(ASTToString(CreateParser(case5)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({
                ASTNode::Negtive(ASTNode::Negtive(ASTNode::Identifier("var1"))),
            })));
}

TEST(Parser, ArithmeticMulAndDiv) {
  std::string case1 = "{+1 * -2;}";
  EXPECT_EQ(
      ASTToString(CreateParser(case1)->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::Multiply(
          ASTNode::Integer("1"), ASTNode::Negtive(ASTNode::Integer("2")))})));
  std::string case2 = "{1 * 2;2 * 2;}";
  EXPECT_EQ(
      ASTToString(CreateParser(case2)->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement(
          {ASTNode::Multiply(ASTNode::Integer("1"), ASTNode::Integer("2")),
           ASTNode::Multiply(ASTNode::Integer("2"), ASTNode::Integer("2"))})));
  std::string case3 = "{+1 * +2 * -3;}";
  EXPECT_EQ(ASTToString(CreateParser(case3)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({ASTNode::Multiply(
                ASTNode::Multiply(ASTNode::Integer("1"), ASTNode::Integer("2")),
                ASTNode::Negtive(ASTNode::Integer("3")))})));
  std::string case4 = "{1 / 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case4)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({ASTNode::Divide(
                ASTNode::Integer("1"), ASTNode::Integer("2"))})));
  std::string case5 = "{1 / 2 * 3;}";
  EXPECT_EQ(ASTToString(CreateParser(case5)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({ASTNode::Multiply(
                ASTNode::Divide(ASTNode::Integer("1"), ASTNode::Integer("2")),
                ASTNode::Integer("3"))})));
}

TEST(Parser, ArithmeticAddAndSub) {
  std::string case1 = "{1 + 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case1)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement(
                {ASTNode::Add(ASTNode::Integer("1"), ASTNode::Integer("2"))})));
  std::string case2 = "{1 + 2;2 + 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case2)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({
                ASTNode::Add(ASTNode::Integer("1"), ASTNode::Integer("2")),
                ASTNode::Add(ASTNode::Integer("2"), ASTNode::Integer("2")),
            })));
  std::string case3 = "{1 + 2 + 3;}";
  EXPECT_EQ(ASTToString(CreateParser(case3)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({ASTNode::Add(
                ASTNode::Add(ASTNode::Integer("1"), ASTNode::Integer("2")),
                ASTNode::Integer("3"))})));
  std::string case4 = "{1 - 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case4)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({ASTNode::Subtract(
                ASTNode::Integer("1"), ASTNode::Integer("2"))})));
  std::string case5 = "{1 - 2 + 3;}";
  EXPECT_EQ(ASTToString(CreateParser(case5)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({ASTNode::Add(
                ASTNode::Subtract(ASTNode::Integer("1"), ASTNode::Integer("2")),
                ASTNode::Integer("3"))})));
  std::string case6 = "{1 - (2 * (var1 + 3)) + (1 + var2);}";
  EXPECT_EQ(
      ASTToString(CreateParser(case6)->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::Add(
          ASTNode::Subtract(
              ASTNode::Integer("1"),
              ASTNode::Multiply(ASTNode::Integer("2"),
                                ASTNode::Add(ASTNode::Identifier("var1"),
                                             ASTNode::Integer("3")))),
          ASTNode::Add(ASTNode::Integer("1"), ASTNode::Identifier("var2")))})));
}

TEST(Parser, ArithmeticRelation) {
  std::string case1 = "{1 > 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case1)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({ASTNode::Less_than(
                ASTNode::Integer("2"), ASTNode::Integer("1"))})));
  std::string case2 = "{1 >= 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case2)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({ASTNode::Less_equal(
                ASTNode::Integer("2"), ASTNode::Integer("1"))})));
  std::string case3 = "{1 < 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case3)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({ASTNode::Less_than(
                ASTNode::Integer("1"), ASTNode::Integer("2"))})));
  std::string case4 = "{1 <= 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case4)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({ASTNode::Less_equal(
                ASTNode::Integer("1"), ASTNode::Integer("2"))})));
  std::string case5 = "{1 < 2 <= 2;}";
  EXPECT_EQ(
      ASTToString(CreateParser(case5)->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::Less_equal(
          ASTNode::Less_than(ASTNode::Integer("1"), ASTNode::Integer("2")),
          ASTNode::Integer("2"))})));
}

TEST(Parser, ArithmeticEquality) {
  std::string case1 = "{1 == 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case1)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({ASTNode::Equal(
                ASTNode::Integer("1"), ASTNode::Integer("2"))})));
  std::string case2 = "{1 != 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case2)->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({ASTNode::Not_equal(
                ASTNode::Integer("1"), ASTNode::Integer("2"))})));
  std::string case3 = "{1 < 2 != 2;}";
  EXPECT_EQ(
      ASTToString(CreateParser(case3)->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::Not_equal(
          ASTNode::Less_than(ASTNode::Integer("1"), ASTNode::Integer("2")),
          ASTNode::Integer("2"))})));
}

TEST(Parser, Assignment) {
  EXPECT_EQ(
      ASTToString(CreateParser("{1 = 2 = 3;}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::Assign(
          ASTNode::Integer("1"),
          ASTNode::Assign(ASTNode::Integer("2"), ASTNode::Integer("3")))})));
  EXPECT_EQ(ASTToString(CreateParser("{1 == 2 = var2 = 3 + var3;}")
                            ->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({ASTNode::Assign(
                ASTNode::Equal(ASTNode::Integer("1"), ASTNode::Integer("2")),
                ASTNode::Assign(ASTNode::Identifier("var2"),
                                ASTNode::Add(ASTNode::Integer("3"),
                                             ASTNode::Identifier("var3"))))})));
}

TEST(Parser, CompoundStatement) {
  EXPECT_EQ(
      ASTToString(
          CreateParser("{ {var1;} {{var2;}} {} }")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement(
          {ASTNode::Compound_statement({ASTNode::Identifier("var1")}),
           ASTNode::Compound_statement(
               {ASTNode::Compound_statement({ASTNode::Identifier("var2")})}),
           ASTNode::Compound_statement({})})));
}

TEST(Parser, SelectStatement) {
  EXPECT_EQ(
      ASTToString(
          CreateParser("{if(var1) { var1 = 1;}}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement(
          {ASTNode::If_else(ASTNode::Identifier("var1"),
                            ASTNode::Compound_statement({
                                ASTNode::Assign(ASTNode::Identifier("var1"),
                                                ASTNode::Integer("1")),
                            }),
                            nullptr)})));
  EXPECT_EQ(ASTToString(CreateParser("{if(var1) { var1 = 1;} else {var1=2;}}")
                            ->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({ASTNode::If_else(
                ASTNode::Identifier("var1"),
                ASTNode::Compound_statement({
                    ASTNode::Assign(ASTNode::Identifier("var1"),
                                    ASTNode::Integer("1")),
                }),
                ASTNode::Compound_statement({
                    ASTNode::Assign(ASTNode::Identifier("var1"),
                                    ASTNode::Integer("2")),
                }))})));
  EXPECT_EQ(
      ASTToString(CreateParser("{if(var1) { var1 = 1;} else if (var2) "
                               "{var2=2;} else {var1 = 3;}}")
                      ->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::If_else(
          ASTNode::Identifier("var1"),
          ASTNode::Compound_statement({
              ASTNode::Assign(ASTNode::Identifier("var1"),
                              ASTNode::Integer("1")),
          }),
          ASTNode::If_else(
              ASTNode::Identifier("var2"),
              ASTNode::Compound_statement({ASTNode::Assign(
                  ASTNode::Identifier("var2"), ASTNode::Integer("2"))}),
              ASTNode::Compound_statement({ASTNode::Assign(
                  ASTNode::Identifier("var1"), ASTNode::Integer("3"))})))})));
}

TEST(Parser, IterationStatement) {
  EXPECT_EQ(
      ASTToString(CreateParser("{for(;;) {}}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::For_loop(
          ASTNode::empty(), ASTNode::empty(), ASTNode::empty(),
          ASTNode::Compound_statement({}))})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{for(var1;var2;var3) {}}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::For_loop(
          ASTNode::Identifier("var1"), ASTNode::Identifier("var2"),
          ASTNode::Identifier("var3"), ASTNode::Compound_statement({}))})));
  EXPECT_EQ(ASTToString(CreateParser("{for(var1; var2; var3) { var1 = 3; } }")
                            ->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({ASTNode::For_loop(
                ASTNode::Identifier("var1"), ASTNode::Identifier("var2"),
                ASTNode::Identifier("var3"),
                ASTNode::Compound_statement({ASTNode::Assign(
                    ASTNode::Identifier("var1"), ASTNode::Integer("3"))}))})));
  EXPECT_EQ(
      ASTToString(CreateParser("{while(var1) {}}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::While_loop(
          ASTNode::Identifier("var1"), ASTNode::Compound_statement({}))})));
  EXPECT_EQ(ASTToString(CreateParser("{while(var1) { var1 = 3; }}")
                            ->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({ASTNode::While_loop(
                ASTNode::Identifier("var1"),
                ASTNode::Compound_statement({ASTNode::Assign(
                    ASTNode::Identifier("var1"), ASTNode::Integer("3"))}))})));
  EXPECT_ANY_THROW(CreateParser("{while() {}}")->parse_compound_statement());
  EXPECT_EQ(ASTToString(CreateParser("{do {var1 = 3;} while(var1);}")
                            ->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement({ASTNode::Do_while(
                ASTNode::Compound_statement({ASTNode::Assign(
                    ASTNode::Identifier("var1"), ASTNode::Integer("3"))}),
                ASTNode::Identifier("var1"))})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{do ; while(var1 = 3);}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::Do_while(
          ASTNode::empty(), ASTNode::Assign(ASTNode::Identifier("var1"),
                                            ASTNode::Integer("3")))})));
  EXPECT_ANY_THROW(
      CreateParser("{do while(var1);}")->parse_compound_statement());
  EXPECT_ANY_THROW(CreateParser("{do ;while();}")->parse_compound_statement());
  EXPECT_ANY_THROW(CreateParser("{do ;while()}")->parse_compound_statement());
}
TEST(Parser, Declaration) {
  EXPECT_THROW(CreateParser("{int (*);}")->parse_compound_statement(),
               std::invalid_argument);
  EXPECT_THROW(CreateParser("{int (*)();}")->parse_compound_statement(),
               std::invalid_argument);
  EXPECT_THROW(CreateParser("{int *();}")->parse_compound_statement(),
               std::invalid_argument);
  EXPECT_THROW(CreateParser("{int *;}")->parse_compound_statement(),
               std::invalid_argument);
  EXPECT_THROW(CreateParser("{int [2];}")->parse_compound_statement(),
               std::invalid_argument);
  EXPECT_THROW(CreateParser("{int a[];}")->parse_compound_statement(),
               std::invalid_argument);
  EXPECT_THROW(CreateParser("{int a[][2];}")->parse_compound_statement(),
               std::invalid_argument);
  EXPECT_THROW(CreateParser("{int a[2]();}")->parse_compound_statement(),
               std::invalid_argument);
  EXPECT_EQ(ASTToString(CreateParser("{int ;}")->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement(
                {ASTNode::Declaration_statement({})})));
  EXPECT_EQ(ASTToString(CreateParser("{int a;}")->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement(
                {ASTNode::Declaration_statement({ASTNode::Variable_declaration(
                    Declarator::Create(Type::basic(TypeKind::INT), "a"),
                    nullptr)})})));
  EXPECT_EQ(ASTToString(CreateParser("{int a();}")->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement(
                {ASTNode::Declaration_statement({ASTNode::Function_declaration(
                    Declarator::Create(
                        Type::function(Type::basic(TypeKind::INT), {}), "a"),
                    nullptr)})})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{int a(int b, int* c);}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement(
          {ASTNode::Declaration_statement({ASTNode::Function_declaration(
              Declarator::Create(
                  Type::function(
                      Type::basic(TypeKind::INT),
                      {Declarator::Create(Type::basic(TypeKind::INT), "b"),
                       Declarator::Create(
                           Type::pointer(Type::basic(TypeKind::INT)), "c")}),
                  "a"),
              nullptr)})})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{int a(int b[2]);}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement(
          {ASTNode::Declaration_statement({ASTNode::Function_declaration(
              Declarator::Create(
                  Type::function(
                      Type::basic(TypeKind::INT),
                      {
                          Declarator::Create(
                              Type::pointer(Type::basic(TypeKind::INT), 2),
                              "b"),
                      }),
                  "a"),
              nullptr)})})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{int a(int b());}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement(
          {ASTNode::Declaration_statement({ASTNode::Function_declaration(
              Declarator::Create(
                  Type::function(Type::basic(TypeKind::INT),
                                 {
                                     Declarator::Create(
                                         Type::pointer(Type::function(
                                             Type::basic(TypeKind::INT), {})),
                                         "b"),
                                 }),
                  "a"),
              nullptr)})})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{int a(int b(int c[]));}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement(
          {ASTNode::Declaration_statement({ASTNode::Function_declaration(
              Declarator::Create(
                  Type::function(
                      Type::basic(TypeKind::INT),
                      {
                          Declarator::Create(
                              Type::pointer(Type::function(
                                  Type::basic(TypeKind::INT),
                                  {Declarator::Create(
                                      Type::pointer(Type::basic(TypeKind::INT)),
                                      "c")})),
                              "b"),
                      }),
                  "a"),
              nullptr)})})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{int a(int b(int c()));}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement(
          {ASTNode::Declaration_statement({ASTNode::Function_declaration(
              Declarator::Create(
                  Type::function(
                      Type::basic(TypeKind::INT),
                      {
                          Declarator::Create(
                              Type::pointer(Type::function(
                                  Type::basic(TypeKind::INT),
                                  {Declarator::Create(
                                      Type::pointer(Type::function(
                                          Type::basic(TypeKind::INT), {})),
                                      "c")})),
                              "b"),
                      }),
                  "a"),
              nullptr)})})));
  EXPECT_EQ(
      ASTToString(CreateParser("{int a[2];}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement(
          {ASTNode::Declaration_statement({ASTNode::Variable_declaration(
              Declarator::Create(Type::array(Type::basic(TypeKind::INT), 2),
                                 "a"),
              nullptr)})})));
  EXPECT_EQ(
      ASTToString(CreateParser("{int a, b;}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::Declaration_statement(
          {ASTNode::Declaration(
               Declarator::Create(Type::basic(TypeKind::INT), "a"), nullptr),
           ASTNode::Declaration(
               Declarator::Create(Type::basic(TypeKind::INT), "b"),
               nullptr)})})));
  EXPECT_EQ(
      ASTToString(CreateParser("{int (*a);}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement(
          {ASTNode::Declaration_statement({ASTNode::Declaration(
              Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                 "a"),
              nullptr)})})));
  EXPECT_EQ(ASTToString(
                CreateParser("{int *(*a[4])();}")->parse_compound_statement()),
            ASTToString(ASTNode::Compound_statement(
                {ASTNode::Declaration_statement({ASTNode::Declaration(
                    Declarator::Create(
                        Type::array(
                            Type::pointer(Type::function(
                                Type::pointer(Type::basic(TypeKind::INT)), {})),
                            4),
                        "a"),
                    nullptr)})})));
  EXPECT_EQ(
      ASTToString(CreateParser("{int **a, *b;}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::Declaration_statement(
          {ASTNode::Declaration(
               Declarator::Create(
                   Type::pointer(Type::pointer(Type::basic(TypeKind::INT))),
                   "a"),
               nullptr),
           ASTNode::Declaration(
               Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                  "b"),
               nullptr)})})));
  EXPECT_EQ(
      ASTToString(CreateParser("{int a[4], *b;}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::Declaration_statement(
          {ASTNode::Declaration(
               Declarator::Create(Type::array(Type::basic(TypeKind::INT), 4),
                                  "a"),
               nullptr),
           ASTNode::Declaration(
               Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                  "b"),
               nullptr)})})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{int a[4][3], *b;}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::Declaration_statement(
          {ASTNode::Declaration(
               Declarator::Create(
                   Type::array(Type::array(Type::basic(TypeKind::INT), 3), 4),
                   "a"),
               nullptr),
           ASTNode::Declaration(
               Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                  "b"),
               nullptr)})})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{int *a[4][3], *b;}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::Declaration_statement(
          {ASTNode::Declaration(
               Declarator::Create(
                   Type::array(
                       Type::array(Type::pointer(Type::basic(TypeKind::INT)),
                                   3),
                       4),
                   "a"),
               nullptr),
           ASTNode::Declaration(
               Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                  "b"),
               nullptr)})})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{int (*a)[4][3], *b;}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::Declaration_statement(
          {ASTNode::Declaration(
               Declarator::Create(
                   Type::pointer(Type::array(
                       Type::array(Type::basic(TypeKind::INT), 3), 4)),
                   "a"),
               nullptr),
           ASTNode::Declaration(
               Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                  "b"),
               nullptr)})})));
  EXPECT_EQ(
      ASTToString(CreateParser("{int a(), *b;}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::Declaration_statement(
          {ASTNode::Declaration(
               Declarator::Create(
                   Type::function(Type::basic(TypeKind::INT), {}), "a"),
               nullptr),
           ASTNode::Declaration(
               Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                  "b"),
               nullptr)})})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{int (*a)(), *b;}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::Declaration_statement(
          {ASTNode::Declaration(
               Declarator::Create(Type::pointer(Type::function(
                                      Type::basic(TypeKind::INT), {})),
                                  "a"),
               nullptr),
           ASTNode::Declaration(
               Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                  "b"),
               nullptr)})})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{int *(*a[4])(), *b;}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::Declaration_statement(
          {ASTNode::Declaration(
               Declarator::Create(
                   Type::array(
                       Type::pointer(Type::function(
                           Type::pointer(Type::basic(TypeKind::INT)), {})),
                       4),
                   "a"),
               nullptr),
           ASTNode::Declaration(
               Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                  "b"),
               nullptr)})})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{int *a = c = b + 10;}")->parse_compound_statement()),
      ASTToString(ASTNode::Compound_statement({ASTNode::Declaration_statement({
          ASTNode::Declaration(
              Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                 "a"),
              ASTNode::Assign(ASTNode::Identifier("c"),
                              ASTNode::Add(ASTNode::Identifier("b"),
                                           ASTNode::Integer("10")))),
      })})));
}

TEST(Parser, DeclarationOrFunctionDefinition) {
  EXPECT_THROW(CreateParser("int*")->parse_declaration_or_function_definition(),
               std::invalid_argument);
  EXPECT_THROW(
      CreateParser("int (*) {}")->parse_declaration_or_function_definition(),
      std::invalid_argument);
  EXPECT_THROW(
      CreateParser("int () {}")->parse_declaration_or_function_definition(),
      std::invalid_argument);
  EXPECT_THROW(CreateParser("int x(y, int z) {}")
                   ->parse_declaration_or_function_definition(),
               std::invalid_argument);
  EXPECT_THROW(CreateParser("int x(y) int y;")
                   ->parse_declaration_or_function_definition(),
               std::invalid_argument);
  EXPECT_THROW(CreateParser("int x() int y;")
                   ->parse_declaration_or_function_definition(),
               std::invalid_argument);
  EXPECT_THROW(CreateParser("int x(y, int z) int y;")
                   ->parse_declaration_or_function_definition(),
               std::invalid_argument);
  EXPECT_THROW(
      CreateParser("int x()();")->parse_declaration_or_function_definition(),
      std::invalid_argument);
  EXPECT_THROW(
      CreateParser("int x()[];")->parse_declaration_or_function_definition(),
      std::invalid_argument);
  EXPECT_THROW(
      CreateParser("int x()[5];")->parse_declaration_or_function_definition(),
      std::invalid_argument);
  EXPECT_EQ(
      ASTToString(
          CreateParser("int;")->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::Declaration_statement({})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("int a;")->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::Declaration_statement({ASTNode::Declaration(
          Declarator::Create(Type::basic(TypeKind::INT), "a"), nullptr)})));
  EXPECT_EQ(
      ASTToString(CreateParser("int a[4];")
                      ->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::Declaration_statement({ASTNode::Declaration(
          Declarator::Create(Type::array(Type::basic(TypeKind::INT), 4), "a"),
          nullptr)})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("int a();")->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::Declaration_statement({ASTNode::Declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}),
                             "a"),
          nullptr)})));
  EXPECT_EQ(ASTToString(CreateParser("int a = b;")
                            ->parse_declaration_or_function_definition()),
            ASTToString(ASTNode::Declaration_statement({ASTNode::Declaration(
                Declarator::Create(Type::basic(TypeKind::INT), "a"),
                ASTNode::Identifier("b"))})));
  EXPECT_EQ(
      ASTToString(CreateParser("int a, *b, c = a = 10;")
                      ->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::Declaration_statement({
          ASTNode::Declaration(
              Declarator::Create(Type::basic(TypeKind::INT), "a"), nullptr),
          ASTNode::Declaration(
              Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                 "b"),
              nullptr),
          ASTNode::Declaration(
              Declarator::Create(Type::basic(TypeKind::INT), "c"),
              ASTNode::Assign(ASTNode::Identifier("a"),
                              ASTNode::Integer("10"))),
      })));
  EXPECT_EQ(ASTToString(CreateParser("int a = 10, *b, c = a = 10;")
                            ->parse_declaration_or_function_definition()),
            ASTToString(ASTNode::Declaration_statement({
                ASTNode::Declaration(
                    Declarator::Create(Type::basic(TypeKind::INT), "a"),
                    ASTNode::Integer("10")),
                ASTNode::Declaration(
                    Declarator::Create(
                        Type::pointer(Type::basic(TypeKind::INT)), "b"),
                    nullptr),
                ASTNode::Declaration(
                    Declarator::Create(Type::basic(TypeKind::INT), "c"),
                    ASTNode::Assign(ASTNode::Identifier("a"),
                                    ASTNode::Integer("10"))),
            })));
  EXPECT_EQ(ASTToString(CreateParser("int a() {}")
                            ->parse_declaration_or_function_definition()),
            ASTToString(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::function(Type::basic(TypeKind::INT), {}), "a"),
                {}, ASTNode::Compound_statement({}))));
  EXPECT_EQ(
      ASTToString(
          CreateParser("a() {}")->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::Function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}),
                             "a"),
          {}, ASTNode::Compound_statement({}))));
  EXPECT_EQ(
      ASTToString(CreateParser("int* a() { int x = y; }")
                      ->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::Function_declaration(
          Declarator::Create(
              Type::function(Type::pointer(Type::basic(TypeKind::INT)), {}),
              "a"),
          {},
          ASTNode::Compound_statement(
              {ASTNode::Declaration_statement({ASTNode::Declaration(
                  Declarator::Create(Type::basic(TypeKind::INT), "x"),
                  ASTNode::Identifier("y"))})}))));
  EXPECT_EQ(
      ASTToString(CreateParser("int* a() { int x = y; }")
                      ->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::Function_declaration(
          Declarator::Create(
              Type::function(Type::pointer(Type::basic(TypeKind::INT)), {}),
              "a"),
          {},
          ASTNode::Compound_statement(
              {ASTNode::Declaration_statement({ASTNode::Declaration(
                  Declarator::Create(Type::basic(TypeKind::INT), "x"),
                  ASTNode::Identifier("y"))})}))));
  EXPECT_EQ(ASTToString(CreateParser("int a(int i, int j);")
                            ->parse_declaration_or_function_definition()),
            ASTToString(ASTNode::Declaration_statement({ASTNode::Declaration(
                Declarator::Create(
                    Type::function(
                        Type::basic(TypeKind::INT),
                        {
                            Declarator::Create(Type::basic(TypeKind::INT), "i"),
                            Declarator::Create(Type::basic(TypeKind::INT), "j"),
                        }),
                    "a"),
                nullptr)})));
  EXPECT_EQ(
      ASTToString(CreateParser("int a(int i(), int j[5]);")
                      ->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::Declaration_statement({ASTNode::Declaration(
          Declarator::Create(
              Type::function(
                  Type::basic(TypeKind::INT),
                  {
                      Declarator::Create(Type::pointer(Type::function(
                                             Type::basic(TypeKind::INT), {})),
                                         "i"),
                      Declarator::Create(
                          Type::pointer(Type::basic(TypeKind::INT), 5), "j"),
                  }),
              "a"),
          nullptr)})));
  EXPECT_EQ(
      ASTToString(CreateParser("int a(int i(), int j[][5]);")
                      ->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::Declaration_statement({ASTNode::Declaration(
          Declarator::Create(
              Type::function(
                  Type::basic(TypeKind::INT),
                  {
                      Declarator::Create(Type::pointer(Type::function(
                                             Type::basic(TypeKind::INT), {})),
                                         "i"),
                      Declarator::Create(Type::pointer(Type::array(
                                             Type::basic(TypeKind::INT), 5)),
                                         "j"),
                  }),
              "a"),
          nullptr)})));
  EXPECT_EQ(ASTToString(CreateParser("int a(i, j);")
                            ->parse_declaration_or_function_definition()),
            ASTToString(ASTNode::Declaration_statement({ASTNode::Declaration(
                Declarator::Create(
                    Type::function(
                        Type::basic(TypeKind::INT),
                        {
                            Declarator::Create(Type::basic(TypeKind::INT), "i"),
                            Declarator::Create(Type::basic(TypeKind::INT), "j"),
                        }),
                    "a"),
                nullptr)})));
  EXPECT_EQ(ASTToString(CreateParser("int a(i, j, k) int k; {}")
                            ->parse_declaration_or_function_definition()),
            ASTToString(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::function(
                        Type::basic(TypeKind::INT),
                        {Declarator::Create(Type::basic(TypeKind::INT), "i"),
                         Declarator::Create(Type::basic(TypeKind::INT), "j"),
                         Declarator::Create(Type::basic(TypeKind::INT), "k")}),
                    "a"),
                ASTNode::Compound_statement({}))));
  EXPECT_EQ(
      ASTToString(CreateParser("int* a(i, j) int i; int j; { int x = y; }")
                      ->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::Function_declaration(
          Declarator::Create(
              Type::function(
                  Type::pointer(Type::basic(TypeKind::INT)),
                  {
                      Declarator::Create(Type::basic(TypeKind::INT), "i"),
                      Declarator::Create(Type::basic(TypeKind::INT), "j"),
                  }),
              "a"),
          ASTNode::Compound_statement(
              {ASTNode::Declaration_statement({ASTNode::Declaration(
                  Declarator::Create(Type::basic(TypeKind::INT), "x"),
                  ASTNode::Identifier("y"))})}))));
  EXPECT_EQ(ASTToString(CreateParser("int* a(int i, int j) { int x = y; }")
                            ->parse_declaration_or_function_definition()),
            ASTToString(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::function(
                        Type::pointer(Type::basic(TypeKind::INT)),
                        {
                            Declarator::Create(Type::basic(TypeKind::INT), "i"),
                            Declarator::Create(Type::basic(TypeKind::INT), "j"),
                        }),
                    "a"),
                {},
                ASTNode::Compound_statement(
                    {ASTNode::Declaration_statement({ASTNode::Declaration(
                        Declarator::Create(Type::basic(TypeKind::INT), "x"),
                        ASTNode::Identifier("y"))})}))));
  EXPECT_EQ(ASTToString(
                CreateParser("int a(int (*i)(int, int), int j) { int x = y; }")
                    ->parse_declaration_or_function_definition()),
            ASTToString(ASTNode::Function_declaration(
                Declarator::Create(
                    Type::function(
                        Type::basic(TypeKind::INT),
                        {
                            Declarator::Create(
                                Type::pointer(Type::function(
                                    Type::basic(TypeKind::INT),
                                    {Declarator::Create(
                                         Type::basic(TypeKind::INT), ""),
                                     Declarator::Create(
                                         Type::basic(TypeKind::INT), "")})),
                                "i"),
                            Declarator::Create(Type::basic(TypeKind::INT), "j"),
                        }),
                    "a"),
                {},
                ASTNode::Compound_statement(
                    {ASTNode::Declaration_statement({ASTNode::Declaration(
                        Declarator::Create(Type::basic(TypeKind::INT), "x"),
                        ASTNode::Identifier("y"))})}))));
}

TEST(Parser, GetAddress) {
    EXPECT_EQ(ASTToString(CreateParser("&a;")->parse_assignment()),
              ASTToString(ASTNode::Get_address(ASTNode::Identifier("a"))));    
}

} // namespace front
