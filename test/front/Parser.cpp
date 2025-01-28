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
            ASTToString(ASTNode::compound_statement({ASTNode::integer("1")})));
  std::string case2 = "{1; 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case2)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement(
                {ASTNode::integer("1"), ASTNode::integer("2")})));
  std::string case3 = "{var1; var2;}";
  EXPECT_EQ(ASTToString(CreateParser(case3)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement(
                {ASTNode::identifier("var1"), ASTNode::identifier("var2")})));
  std::string case4 = "{(var1);}";
  EXPECT_EQ(
      ASTToString(CreateParser(case4)->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::identifier("var1")})));
}

TEST(Parser, ArithmeticUnary) {
  std::string case1 = "{-1;}";
  EXPECT_EQ(ASTToString(CreateParser(case1)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement(
                {ASTNode::negtive(ASTNode::integer("1"))})));
  std::string case2 = "{-1; -2;}";
  EXPECT_EQ(ASTToString(CreateParser(case2)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement(
                {ASTNode::negtive(ASTNode::integer("1")),
                 ASTNode::negtive(ASTNode::integer("2"))})));
  std::string case3 = "{-var1; -var2;}";
  EXPECT_EQ(ASTToString(CreateParser(case3)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement(
                {ASTNode::negtive(ASTNode::identifier("var1")),
                 ASTNode::negtive(ASTNode::identifier("var2"))})));
  std::string case4 = "{(-var1);}";
  EXPECT_EQ(ASTToString(CreateParser(case4)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({
                ASTNode::negtive(ASTNode::identifier("var1")),
            })));
  std::string case5 = "{-(-var1);}";
  EXPECT_EQ(ASTToString(CreateParser(case5)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({
                ASTNode::negtive(ASTNode::negtive(ASTNode::identifier("var1"))),
            })));
}

TEST(Parser, ArithmeticMulAndDiv) {
  std::string case1 = "{+1 * -2;}";
  EXPECT_EQ(
      ASTToString(CreateParser(case1)->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::multiply(
          ASTNode::integer("1"), ASTNode::negtive(ASTNode::integer("2")))})));
  std::string case2 = "{1 * 2;2 * 2;}";
  EXPECT_EQ(
      ASTToString(CreateParser(case2)->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement(
          {ASTNode::multiply(ASTNode::integer("1"), ASTNode::integer("2")),
           ASTNode::multiply(ASTNode::integer("2"), ASTNode::integer("2"))})));
  std::string case3 = "{+1 * +2 * -3;}";
  EXPECT_EQ(ASTToString(CreateParser(case3)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({ASTNode::multiply(
                ASTNode::multiply(ASTNode::integer("1"), ASTNode::integer("2")),
                ASTNode::negtive(ASTNode::integer("3")))})));
  std::string case4 = "{1 / 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case4)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({ASTNode::divide(
                ASTNode::integer("1"), ASTNode::integer("2"))})));
  std::string case5 = "{1 / 2 * 3;}";
  EXPECT_EQ(ASTToString(CreateParser(case5)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({ASTNode::multiply(
                ASTNode::divide(ASTNode::integer("1"), ASTNode::integer("2")),
                ASTNode::integer("3"))})));
}

TEST(Parser, ArithmeticAddAndSub) {
  std::string case1 = "{1 + 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case1)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement(
                {ASTNode::add(ASTNode::integer("1"), ASTNode::integer("2"))})));
  std::string case2 = "{1 + 2;2 + 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case2)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({
                ASTNode::add(ASTNode::integer("1"), ASTNode::integer("2")),
                ASTNode::add(ASTNode::integer("2"), ASTNode::integer("2")),
            })));
  std::string case3 = "{1 + 2 + 3;}";
  EXPECT_EQ(ASTToString(CreateParser(case3)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({ASTNode::add(
                ASTNode::add(ASTNode::integer("1"), ASTNode::integer("2")),
                ASTNode::integer("3"))})));
  std::string case4 = "{1 - 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case4)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({ASTNode::subtract(
                ASTNode::integer("1"), ASTNode::integer("2"))})));
  std::string case5 = "{1 - 2 + 3;}";
  EXPECT_EQ(ASTToString(CreateParser(case5)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({ASTNode::add(
                ASTNode::subtract(ASTNode::integer("1"), ASTNode::integer("2")),
                ASTNode::integer("3"))})));
  std::string case6 = "{1 - (2 * (var1 + 3)) + (1 + var2);}";
  EXPECT_EQ(
      ASTToString(CreateParser(case6)->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::add(
          ASTNode::subtract(
              ASTNode::integer("1"),
              ASTNode::multiply(ASTNode::integer("2"),
                                ASTNode::add(ASTNode::identifier("var1"),
                                             ASTNode::integer("3")))),
          ASTNode::add(ASTNode::integer("1"), ASTNode::identifier("var2")))})));
}

TEST(Parser, ArithmeticRelation) {
  std::string case1 = "{1 > 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case1)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({ASTNode::less_than(
                ASTNode::integer("2"), ASTNode::integer("1"))})));
  std::string case2 = "{1 >= 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case2)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({ASTNode::less_equal(
                ASTNode::integer("2"), ASTNode::integer("1"))})));
  std::string case3 = "{1 < 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case3)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({ASTNode::less_than(
                ASTNode::integer("1"), ASTNode::integer("2"))})));
  std::string case4 = "{1 <= 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case4)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({ASTNode::less_equal(
                ASTNode::integer("1"), ASTNode::integer("2"))})));
  std::string case5 = "{1 < 2 <= 2;}";
  EXPECT_EQ(
      ASTToString(CreateParser(case5)->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::less_equal(
          ASTNode::less_than(ASTNode::integer("1"), ASTNode::integer("2")),
          ASTNode::integer("2"))})));
}

TEST(Parser, ArithmeticEquality) {
  std::string case1 = "{1 == 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case1)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({ASTNode::equal(
                ASTNode::integer("1"), ASTNode::integer("2"))})));
  std::string case2 = "{1 != 2;}";
  EXPECT_EQ(ASTToString(CreateParser(case2)->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({ASTNode::not_equal(
                ASTNode::integer("1"), ASTNode::integer("2"))})));
  std::string case3 = "{1 < 2 != 2;}";
  EXPECT_EQ(
      ASTToString(CreateParser(case3)->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::not_equal(
          ASTNode::less_than(ASTNode::integer("1"), ASTNode::integer("2")),
          ASTNode::integer("2"))})));
}

TEST(Parser, Assignment) {
  EXPECT_EQ(
      ASTToString(CreateParser("{1 = 2 = 3;}")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::assign(
          ASTNode::integer("1"),
          ASTNode::assign(ASTNode::integer("2"), ASTNode::integer("3")))})));
  EXPECT_EQ(ASTToString(CreateParser("{1 == 2 = var2 = 3 + var3;}")
                            ->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({ASTNode::assign(
                ASTNode::equal(ASTNode::integer("1"), ASTNode::integer("2")),
                ASTNode::assign(ASTNode::identifier("var2"),
                                ASTNode::add(ASTNode::integer("3"),
                                             ASTNode::identifier("var3"))))})));
}

TEST(Parser, CompoundStatement) {
  EXPECT_EQ(
      ASTToString(
          CreateParser("{ {var1;} {{var2;}} {} }")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement(
          {ASTNode::compound_statement({ASTNode::identifier("var1")}),
           ASTNode::compound_statement(
               {ASTNode::compound_statement({ASTNode::identifier("var2")})}),
           ASTNode::compound_statement({})})));
}

TEST(Parser, SelectStatement) {
  EXPECT_EQ(
      ASTToString(
          CreateParser("{if(var1) { var1 = 1;}}")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement(
          {ASTNode::if_else(ASTNode::identifier("var1"),
                            ASTNode::compound_statement({
                                ASTNode::assign(ASTNode::identifier("var1"),
                                                ASTNode::integer("1")),
                            }),
                            nullptr)})));
  EXPECT_EQ(ASTToString(CreateParser("{if(var1) { var1 = 1;} else {var1=2;}}")
                            ->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({ASTNode::if_else(
                ASTNode::identifier("var1"),
                ASTNode::compound_statement({
                    ASTNode::assign(ASTNode::identifier("var1"),
                                    ASTNode::integer("1")),
                }),
                ASTNode::compound_statement({
                    ASTNode::assign(ASTNode::identifier("var1"),
                                    ASTNode::integer("2")),
                }))})));
  EXPECT_EQ(
      ASTToString(CreateParser("{if(var1) { var1 = 1;} else if (var2) "
                               "{var2=2;} else {var1 = 3;}}")
                      ->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::if_else(
          ASTNode::identifier("var1"),
          ASTNode::compound_statement({
              ASTNode::assign(ASTNode::identifier("var1"),
                              ASTNode::integer("1")),
          }),
          ASTNode::if_else(
              ASTNode::identifier("var2"),
              ASTNode::compound_statement({ASTNode::assign(
                  ASTNode::identifier("var2"), ASTNode::integer("2"))}),
              ASTNode::compound_statement({ASTNode::assign(
                  ASTNode::identifier("var1"), ASTNode::integer("3"))})))})));
}

TEST(Parser, IterationStatement) {
  EXPECT_EQ(
      ASTToString(CreateParser("{for(;;) {}}")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::for_loop(
          ASTNode::empty(), ASTNode::empty(), ASTNode::empty(),
          ASTNode::compound_statement({}))})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{for(var1;var2;var3) {}}")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::for_loop(
          ASTNode::identifier("var1"), ASTNode::identifier("var2"),
          ASTNode::identifier("var3"), ASTNode::compound_statement({}))})));
  EXPECT_EQ(ASTToString(CreateParser("{for(var1; var2; var3) { var1 = 3; } }")
                            ->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({ASTNode::for_loop(
                ASTNode::identifier("var1"), ASTNode::identifier("var2"),
                ASTNode::identifier("var3"),
                ASTNode::compound_statement({ASTNode::assign(
                    ASTNode::identifier("var1"), ASTNode::integer("3"))}))})));
  EXPECT_EQ(
      ASTToString(CreateParser("{while(var1) {}}")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::while_loop(
          ASTNode::identifier("var1"), ASTNode::compound_statement({}))})));
  EXPECT_EQ(ASTToString(CreateParser("{while(var1) { var1 = 3; }}")
                            ->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({ASTNode::while_loop(
                ASTNode::identifier("var1"),
                ASTNode::compound_statement({ASTNode::assign(
                    ASTNode::identifier("var1"), ASTNode::integer("3"))}))})));
  EXPECT_ANY_THROW(CreateParser("{while() {}}")->parse_compound_statement());
  EXPECT_EQ(ASTToString(CreateParser("{do {var1 = 3;} while(var1);}")
                            ->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement({ASTNode::do_while(
                ASTNode::compound_statement({ASTNode::assign(
                    ASTNode::identifier("var1"), ASTNode::integer("3"))}),
                ASTNode::identifier("var1"))})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{do ; while(var1 = 3);}")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::do_while(
          ASTNode::empty(), ASTNode::assign(ASTNode::identifier("var1"),
                                            ASTNode::integer("3")))})));
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
            ASTToString(ASTNode::compound_statement(
                {ASTNode::declaration_statement({})})));
  EXPECT_EQ(ASTToString(CreateParser("{int a;}")->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement(
                {ASTNode::declaration_statement({ASTNode::variable_declaration(
                    Declarator::Create(Type::basic(TypeKind::INT), "a"),
                    nullptr)})})));
  EXPECT_EQ(ASTToString(CreateParser("{int a();}")->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement(
                {ASTNode::declaration_statement({ASTNode::function_declaration(
                    Declarator::Create(
                        Type::function(Type::basic(TypeKind::INT), {}), "a"),
                    nullptr)})})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{int a(int b, int* c);}")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement(
          {ASTNode::declaration_statement({ASTNode::function_declaration(
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
      ASTToString(ASTNode::compound_statement(
          {ASTNode::declaration_statement({ASTNode::function_declaration(
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
      ASTToString(ASTNode::compound_statement(
          {ASTNode::declaration_statement({ASTNode::function_declaration(
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
      ASTToString(ASTNode::compound_statement(
          {ASTNode::declaration_statement({ASTNode::function_declaration(
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
      ASTToString(ASTNode::compound_statement(
          {ASTNode::declaration_statement({ASTNode::function_declaration(
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
      ASTToString(ASTNode::compound_statement(
          {ASTNode::declaration_statement({ASTNode::variable_declaration(
              Declarator::Create(Type::array(Type::basic(TypeKind::INT), 2),
                                 "a"),
              nullptr)})})));
  EXPECT_EQ(
      ASTToString(CreateParser("{int a, b;}")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::declaration_statement(
          {ASTNode::declaration(
               Declarator::Create(Type::basic(TypeKind::INT), "a"), nullptr),
           ASTNode::declaration(
               Declarator::Create(Type::basic(TypeKind::INT), "b"),
               nullptr)})})));
  EXPECT_EQ(
      ASTToString(CreateParser("{int (*a);}")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement(
          {ASTNode::declaration_statement({ASTNode::declaration(
              Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                 "a"),
              nullptr)})})));
  EXPECT_EQ(ASTToString(
                CreateParser("{int *(*a[4])();}")->parse_compound_statement()),
            ASTToString(ASTNode::compound_statement(
                {ASTNode::declaration_statement({ASTNode::declaration(
                    Declarator::Create(
                        Type::array(
                            Type::pointer(Type::function(
                                Type::pointer(Type::basic(TypeKind::INT)), {})),
                            4),
                        "a"),
                    nullptr)})})));
  EXPECT_EQ(
      ASTToString(CreateParser("{int **a, *b;}")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::declaration_statement(
          {ASTNode::declaration(
               Declarator::Create(
                   Type::pointer(Type::pointer(Type::basic(TypeKind::INT))),
                   "a"),
               nullptr),
           ASTNode::declaration(
               Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                  "b"),
               nullptr)})})));
  EXPECT_EQ(
      ASTToString(CreateParser("{int a[4], *b;}")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::declaration_statement(
          {ASTNode::declaration(
               Declarator::Create(Type::array(Type::basic(TypeKind::INT), 4),
                                  "a"),
               nullptr),
           ASTNode::declaration(
               Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                  "b"),
               nullptr)})})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{int a[4][3], *b;}")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::declaration_statement(
          {ASTNode::declaration(
               Declarator::Create(
                   Type::array(Type::array(Type::basic(TypeKind::INT), 3), 4),
                   "a"),
               nullptr),
           ASTNode::declaration(
               Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                  "b"),
               nullptr)})})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{int *a[4][3], *b;}")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::declaration_statement(
          {ASTNode::declaration(
               Declarator::Create(
                   Type::array(
                       Type::array(Type::pointer(Type::basic(TypeKind::INT)),
                                   3),
                       4),
                   "a"),
               nullptr),
           ASTNode::declaration(
               Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                  "b"),
               nullptr)})})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{int (*a)[4][3], *b;}")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::declaration_statement(
          {ASTNode::declaration(
               Declarator::Create(
                   Type::pointer(Type::array(
                       Type::array(Type::basic(TypeKind::INT), 3), 4)),
                   "a"),
               nullptr),
           ASTNode::declaration(
               Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                  "b"),
               nullptr)})})));
  EXPECT_EQ(
      ASTToString(CreateParser("{int a(), *b;}")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::declaration_statement(
          {ASTNode::declaration(
               Declarator::Create(
                   Type::function(Type::basic(TypeKind::INT), {}), "a"),
               nullptr),
           ASTNode::declaration(
               Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                  "b"),
               nullptr)})})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{int (*a)(), *b;}")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::declaration_statement(
          {ASTNode::declaration(
               Declarator::Create(Type::pointer(Type::function(
                                      Type::basic(TypeKind::INT), {})),
                                  "a"),
               nullptr),
           ASTNode::declaration(
               Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                  "b"),
               nullptr)})})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{int *(*a[4])(), *b;}")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::declaration_statement(
          {ASTNode::declaration(
               Declarator::Create(
                   Type::array(
                       Type::pointer(Type::function(
                           Type::pointer(Type::basic(TypeKind::INT)), {})),
                       4),
                   "a"),
               nullptr),
           ASTNode::declaration(
               Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                  "b"),
               nullptr)})})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("{int *a = c = b + 10;}")->parse_compound_statement()),
      ASTToString(ASTNode::compound_statement({ASTNode::declaration_statement({
          ASTNode::declaration(
              Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                 "a"),
              ASTNode::assign(ASTNode::identifier("c"),
                              ASTNode::add(ASTNode::identifier("b"),
                                           ASTNode::integer("10")))),
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
      ASTToString(ASTNode::declaration_statement({})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("int a;")->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::declaration_statement({ASTNode::declaration(
          Declarator::Create(Type::basic(TypeKind::INT), "a"), nullptr)})));
  EXPECT_EQ(
      ASTToString(CreateParser("int a[4];")
                      ->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::declaration_statement({ASTNode::declaration(
          Declarator::Create(Type::array(Type::basic(TypeKind::INT), 4), "a"),
          nullptr)})));
  EXPECT_EQ(
      ASTToString(
          CreateParser("int a();")->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::declaration_statement({ASTNode::declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}),
                             "a"),
          nullptr)})));
  EXPECT_EQ(ASTToString(CreateParser("int a = b;")
                            ->parse_declaration_or_function_definition()),
            ASTToString(ASTNode::declaration_statement({ASTNode::declaration(
                Declarator::Create(Type::basic(TypeKind::INT), "a"),
                ASTNode::identifier("b"))})));
  EXPECT_EQ(
      ASTToString(CreateParser("int a, *b, c = a = 10;")
                      ->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::declaration_statement({
          ASTNode::declaration(
              Declarator::Create(Type::basic(TypeKind::INT), "a"), nullptr),
          ASTNode::declaration(
              Declarator::Create(Type::pointer(Type::basic(TypeKind::INT)),
                                 "b"),
              nullptr),
          ASTNode::declaration(
              Declarator::Create(Type::basic(TypeKind::INT), "c"),
              ASTNode::assign(ASTNode::identifier("a"),
                              ASTNode::integer("10"))),
      })));
  EXPECT_EQ(ASTToString(CreateParser("int a = 10, *b, c = a = 10;")
                            ->parse_declaration_or_function_definition()),
            ASTToString(ASTNode::declaration_statement({
                ASTNode::declaration(
                    Declarator::Create(Type::basic(TypeKind::INT), "a"),
                    ASTNode::integer("10")),
                ASTNode::declaration(
                    Declarator::Create(
                        Type::pointer(Type::basic(TypeKind::INT)), "b"),
                    nullptr),
                ASTNode::declaration(
                    Declarator::Create(Type::basic(TypeKind::INT), "c"),
                    ASTNode::assign(ASTNode::identifier("a"),
                                    ASTNode::integer("10"))),
            })));
  EXPECT_EQ(ASTToString(CreateParser("int a() {}")
                            ->parse_declaration_or_function_definition()),
            ASTToString(ASTNode::function_declaration(
                Declarator::Create(
                    Type::function(Type::basic(TypeKind::INT), {}), "a"),
                {}, ASTNode::compound_statement({}))));
  EXPECT_EQ(
      ASTToString(
          CreateParser("a() {}")->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::function_declaration(
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}),
                             "a"),
          {}, ASTNode::compound_statement({}))));
  EXPECT_EQ(
      ASTToString(CreateParser("int* a() { int x = y; }")
                      ->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::function_declaration(
          Declarator::Create(
              Type::function(Type::pointer(Type::basic(TypeKind::INT)), {}),
              "a"),
          {},
          ASTNode::compound_statement(
              {ASTNode::declaration_statement({ASTNode::declaration(
                  Declarator::Create(Type::basic(TypeKind::INT), "x"),
                  ASTNode::identifier("y"))})}))));
  EXPECT_EQ(
      ASTToString(CreateParser("int* a() { int x = y; }")
                      ->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::function_declaration(
          Declarator::Create(
              Type::function(Type::pointer(Type::basic(TypeKind::INT)), {}),
              "a"),
          {},
          ASTNode::compound_statement(
              {ASTNode::declaration_statement({ASTNode::declaration(
                  Declarator::Create(Type::basic(TypeKind::INT), "x"),
                  ASTNode::identifier("y"))})}))));
  EXPECT_EQ(ASTToString(CreateParser("int a(int i, int j);")
                            ->parse_declaration_or_function_definition()),
            ASTToString(ASTNode::declaration_statement({ASTNode::declaration(
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
      ASTToString(ASTNode::declaration_statement({ASTNode::declaration(
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
      ASTToString(ASTNode::declaration_statement({ASTNode::declaration(
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
            ASTToString(ASTNode::declaration_statement({ASTNode::declaration(
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
            ASTToString(ASTNode::function_declaration(
                Declarator::Create(
                    Type::function(
                        Type::basic(TypeKind::INT),
                        {Declarator::Create(Type::basic(TypeKind::INT), "i"),
                         Declarator::Create(Type::basic(TypeKind::INT), "j"),
                         Declarator::Create(Type::basic(TypeKind::INT), "k")}),
                    "a"),
                ASTNode::compound_statement({}))));
  EXPECT_EQ(
      ASTToString(CreateParser("int* a(i, j) int i; int j; { int x = y; }")
                      ->parse_declaration_or_function_definition()),
      ASTToString(ASTNode::function_declaration(
          Declarator::Create(
              Type::function(
                  Type::pointer(Type::basic(TypeKind::INT)),
                  {
                      Declarator::Create(Type::basic(TypeKind::INT), "i"),
                      Declarator::Create(Type::basic(TypeKind::INT), "j"),
                  }),
              "a"),
          ASTNode::compound_statement(
              {ASTNode::declaration_statement({ASTNode::declaration(
                  Declarator::Create(Type::basic(TypeKind::INT), "x"),
                  ASTNode::identifier("y"))})}))));
  EXPECT_EQ(ASTToString(CreateParser("int* a(int i, int j) { int x = y; }")
                            ->parse_declaration_or_function_definition()),
            ASTToString(ASTNode::function_declaration(
                Declarator::Create(
                    Type::function(
                        Type::pointer(Type::basic(TypeKind::INT)),
                        {
                            Declarator::Create(Type::basic(TypeKind::INT), "i"),
                            Declarator::Create(Type::basic(TypeKind::INT), "j"),
                        }),
                    "a"),
                {},
                ASTNode::compound_statement(
                    {ASTNode::declaration_statement({ASTNode::declaration(
                        Declarator::Create(Type::basic(TypeKind::INT), "x"),
                        ASTNode::identifier("y"))})}))));
  EXPECT_EQ(ASTToString(
                CreateParser("int a(int (*i)(int, int), int j) { int x = y; }")
                    ->parse_declaration_or_function_definition()),
            ASTToString(ASTNode::function_declaration(
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
                ASTNode::compound_statement(
                    {ASTNode::declaration_statement({ASTNode::declaration(
                        Declarator::Create(Type::basic(TypeKind::INT), "x"),
                        ASTNode::identifier("y"))})}))));
}

} // namespace front
