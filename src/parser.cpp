#include "parser.h"
#include "lexer.h"
#include "AST.h"

#include <set>

class ParserImpl :public Parser {
public:
  ParserImpl(std::istream& input)
    : Parser(input) {
    lexer_  = CreateLexer(input);
  }
  ASTNodePtr work() override;
  ASTNodePtr parse_compound_statement() override;
  ASTNodePtr parse_declaration_or_function_definition() override;
private:
  ASTNodePtr parse_translation_unit();
  ASTNodePtr parse_expression_statement();
  ASTNodePtr parse_select_statement();
  ASTNodePtr parse_iteration_statement();
  ASTNodePtr parse_statement();
  ASTNodePtr parse_expression();
  ASTNodePtr parse_assignment();
  ASTNodePtr parse_relation();
  ASTNodePtr parse_equality();
  ASTNodePtr parse_add_and_subtraction();
  ASTNodePtr parse_multiply_and_division();
  ASTNodePtr parse_unary();
  ASTNodePtr parse_primary();

  std::vector<DeclaratorPtr>          parse_parameter_type_list();
  std::vector<DeclaratorPtr>          parse_identifier_list();
  TypePtr                             parse_type_specifier();
  void                                parse_pointer(TypeBuilderPtr builder);
  ASTNodePtr                          parse_constant_expression();
  ASTNodePtr                          parse_direct_declatator(TypeBuilderPtr builder, bool expect_identifier);
  void                                parse_function_array_suffix(TypeBuilderPtr builder);
  std::vector<DeclarationStatementNodePtr>     parse_declaration_list();
  DeclarationNodePtr                  parse_init_declarator(TypePtr base_type);
  ASTNodePtr                          parse_declarator(TypeBuilderPtr builder, bool expect_identifier);
  DeclarationStatementNodePtr         parse_declaration();
  DeclarationStatementNodePtr         parse_declaration(TypePtr base_type, DeclaratorPtr parsed_declarator);
  std::unique_ptr<Lexer> lexer_;
};

ASTNodePtr ParserImpl::work() {
  return parse_translation_unit();
}

// TRANSLATION_UINT => COMPOUND_STATEMENT
ASTNodePtr ParserImpl::parse_translation_unit() {
  auto result = parse_declaration_or_function_definition();
  TokenPtr next_token = lexer_->next();
  if (next_token->type_ != TokenType::ED) {
    throw std::invalid_argument("EOF expected!");
  }
  return result;
}

// DECLARATION_OR_FUNCTION_DEFINATION =>  TYPE_SPECIFIER DECLARATOR DECLARATION_LIST COMPOUND_STATEMENT
//                                      | TYPE_SPECIFIER DECLARATOR COMPOUND_STATEMENT
//                                      | TYPE_SPECIFIER { INIT_DECLARATOR { ',' INIT_DECLARATOR }* }? ';'
//                                      | DECLARATOR DECLARATION_LIST COMPOUND_STATEMENT
//                                      | DECLARATOR COMPOUND_STATEMENT
ASTNodePtr ParserImpl::parse_declaration_or_function_definition() {
  auto next_token = lexer_->peek();
  if (next_token->type_ == TokenType::TYPE_SPECIFER) {
    auto base_type = parse_type_specifier();
    if (lexer_->peek()->type_ == TokenType::SEMICOLON) {
      return parse_declaration(std::move(base_type), nullptr);
    }
    auto type_builder = CreateTypeBuilder(base_type);
    auto identifier = parse_declarator(type_builder, true);
    next_token = lexer_->peek();
    // Declaration
    if (next_token->type_ == TokenType::COMMA ||
        next_token->type_ == TokenType::SEMICOLON ||
        next_token->type_ == TokenType::ASSIGN) {
      return parse_declaration(std::move(base_type),
                              Declarator::Create(type_builder->get(), identifier->literal_));
    }
    // function definition
    auto declaration_list = parse_declaration_list();
    auto function_body = parse_compound_statement();

    return 
      ASTNode::normalize_declaration(
        ASTNode::function_declaration(Declarator::Create(type_builder->get(), identifier->literal_),
                                          std::move(declaration_list),
                                          std::move(function_body)));
  } else {
    auto type_builder = CreateTypeBuilder(Type::default_type());
    auto identifier = parse_declarator(type_builder, true);
    auto declaration_list = parse_declaration_list();
    auto function_body = parse_compound_statement();
    auto declarator = Declarator::Create(type_builder->get(), identifier->literal_);
    return ASTNode::normalize_declaration(
              ASTNode::function_declaration(std::move(declarator), std::move(declaration_list), std::move(function_body)));
  }
}

//DECLARATION_LIST => { DECLARATION }*
std::vector<DeclarationStatementNodePtr> ParserImpl::parse_declaration_list() {
  std::vector<DeclarationStatementNodePtr> declarations;
  while (true) {
    auto next_token = lexer_->peek();
    if (next_token->type_ != TokenType::TYPE_SPECIFER) {
      return declarations;
    }
    declarations.emplace_back(parse_declaration());
  }
}

// TYPE_SPECIFIER => type_specifier
TypePtr ParserImpl::parse_type_specifier() {
  auto type_specifier = lexer_->next();
  if (type_specifier->literal_ == "int") {
    return Type::basic(TypeKind::INT);
  }
  throw std::invalid_argument("Type specifier expected");
}

DeclarationNodePtr ParserImpl::parse_init_declarator(TypePtr base_type) {
  auto type_builder = CreateTypeBuilder(base_type);
  auto identifier = parse_declarator(type_builder, true);
  ASTNodePtr initializer = nullptr;
  if (lexer_->peek()->type_ == TokenType::ASSIGN) {
    lexer_->expect_next("=");
    initializer = parse_assignment();
  }
  return ASTNode::normalize_declaration(
            ASTNode::declaration(Declarator::Create(type_builder->get(), identifier->literal_),
                                 std::move(initializer)));
}

void ParserImpl::parse_pointer(TypeBuilderPtr builder) {
  while (true) {
    auto next_token = lexer_->peek();
    if (next_token->type_ != TokenType::ASTERISK) { //*
      break;
    }
    lexer_->expect_next("*");
    builder->pointer_of();
  }
}

ASTNodePtr ParserImpl::parse_constant_expression() {
  return parse_equality();
}

std::vector<DeclaratorPtr> ParserImpl::parse_parameter_type_list() {
  std::vector<DeclaratorPtr> result;
  bool expect_comma = false;
  while (true) {
    auto next_token = lexer_->peek();
    if (next_token->type_ == TokenType::RIGHT_PARENTHESE) {
      return result;
    }
    if (expect_comma) {
      lexer_->expect_next(",");
    }
    auto base_type = parse_type_specifier();
    auto type_builder = CreateTypeBuilder(base_type);
    auto identifier = parse_declarator(type_builder, false);
    auto declarator = Declarator::Create(type_builder->get(), identifier->literal_);
    result.emplace_back(std::move(declarator));
    expect_comma = true;
  } 
}

std::vector<DeclaratorPtr> ParserImpl::parse_identifier_list() {
  std::vector<DeclaratorPtr> result;
  bool expect_comma = false;
  while (true) {
    auto next_token = lexer_->peek();
    if (next_token->type_ == TokenType::RIGHT_PARENTHESE) {
      return result;
    }
    if (expect_comma) {
      lexer_->expect_next(",");
    }
    next_token = lexer_->next();
    if (next_token->type_ != TokenType::IDENTIFIER) {
      throw std::invalid_argument("Identifier expected");
    }
    auto declarator = Declarator::Create(nullptr, next_token->literal_);
    result.emplace_back(std::move(declarator));
    expect_comma = true;
  } 
}

// FUNCTION_ARRAY_SUFFIX =>   EMMPTY
//                          | '[' CONSTANT_EXPRESSION ']'
//                          | '(' ')'
//                          | '( PARAMETER_TYPE_LIST ')'
//                          | '( IDENTIFIER_LIST ')'
void ParserImpl::parse_function_array_suffix(TypeBuilderPtr builder) {
  auto next_token = lexer_->peek();
  if (next_token->type_ == TokenType::LEFT_PARENTHESE) { // (
    lexer_->expect_next("(");
    next_token = lexer_->peek();
    std::vector<DeclaratorPtr> parameter_list;
    if (next_token->type_ == TokenType::IDENTIFIER) {
      parameter_list = parse_identifier_list();
    } else if (next_token->type_ == TokenType::TYPE_SPECIFER) {
      parameter_list = parse_parameter_type_list();
    }
    lexer_->expect_next(")");
    parse_function_array_suffix(builder);
    builder->return_of(parameter_list);
  } else if (next_token->type_ == TokenType::LEFT_BRACKET) { // [
    lexer_->expect_next("[");
    int64_t element_count = ArrayType::ELEMENT_COUNT_UNKOWN;
    if (lexer_->peek()->type_ != TokenType::RIGHT_BRACKET) {
      auto element_count_expr = parse_constant_expression();
      if (element_count_expr->kind_ != ASTNodeKind::INTEGER) {
        throw std::invalid_argument("Element count of array should be a integer.");
      }
      element_count = std::stoll(element_count_expr->literal_);
      if (element_count < 0) {
        throw std::invalid_argument("declared an array with a negative size");
      }
    }
    lexer_->expect_next("]");
    parse_function_array_suffix(builder);
    builder->array_of(element_count);
  }
}

// DIRECT_DECLATATOR => {VARIABLE | '(' DECLARATOR ')'}? FUNCTION_ARRAY_SUFFIX
ASTNodePtr ParserImpl::parse_direct_declatator(TypeBuilderPtr builder, bool expect_identifier) {
  auto next_token = lexer_->peek();
  ASTNodePtr result = ASTNode::identifier("");
  if (next_token->type_ == TokenType::IDENTIFIER) {
    next_token = lexer_->next();
    result = ASTNode::identifier(next_token->literal_);
  } else if (next_token->type_ == TokenType::LEFT_PARENTHESE) {
    lexer_->expect_next("(");
    auto old_builder = builder->building_of();
    result = parse_declarator(builder, expect_identifier);
    builder = std::move(old_builder);
    lexer_->expect_next(")");
  } else if (expect_identifier) {
    throw std::invalid_argument("Identifier or ( expected");
  }

  parse_function_array_suffix(builder);
  return result;
}

// DECLARATOR => POINTER? DIRECT_DECLATATOR
ASTNodePtr ParserImpl::parse_declarator(TypeBuilderPtr builder, bool expect_identifier) {
  parse_pointer(builder);
  return parse_direct_declatator(builder, expect_identifier);
}

// DECLARATION => TYPE_SPECIFIER {INIT_DECLARATOR {',' INIT_DECLARATOR}*}? ';'
DeclarationStatementNodePtr ParserImpl::parse_declaration(TypePtr base_type, DeclaratorPtr parsed_declarator) {
  if (base_type == nullptr) {
    base_type = parse_type_specifier();
  }
  std::vector<DeclarationNodePtr> declaration_list;
  if (parsed_declarator != nullptr) {
    ASTNodePtr initializer = nullptr;
    auto next_token = lexer_->peek();
    if (next_token->type_ == TokenType::ASSIGN) {
      lexer_->expect_next("=");
      initializer = parse_assignment();
      next_token = lexer_->peek();
      if (next_token->type_ == TokenType::COMMA) {
        lexer_->expect_next(",");
      }
    } else if (next_token->type_ == TokenType::COMMA) {
      lexer_->expect_next(",");
    }
    declaration_list.emplace_back(
        ASTNode::normalize_declaration(ASTNode::declaration(parsed_declarator, std::move(initializer))));
  }
  while (true)  {
    auto next_token = lexer_->peek();
    if (next_token->type_ == TokenType::SEMICOLON) {
      lexer_->expect_next(";");
      return ASTNode::declaration_statement(std::move(declaration_list));
    }
    declaration_list.emplace_back(parse_init_declarator(base_type));
    next_token = lexer_->peek();
    if (next_token->type_ == TokenType::SEMICOLON) {
      lexer_->expect_next(";");
      return ASTNode::declaration_statement(std::move(declaration_list));
    }
    lexer_->expect_next(",");
  } 
}

DeclarationStatementNodePtr ParserImpl::parse_declaration() {
  return parse_declaration(nullptr, nullptr);
}

// STATEMENT =>  COMPOUND_STATEMENT
//             | EXPRESSION_STATEMENT
//             | SELECT_STATEMENT
//             | ITERATION_STATEMENT
ASTNodePtr ParserImpl::parse_statement() {
  TokenPtr next_token = lexer_->peek();
  if (next_token->type_ == TokenType::LEFT_BRACE) {
    return parse_compound_statement();
  } else if (next_token->type_ == TokenType::KEYWORD &&
             next_token->literal_ == "if") {
    return parse_select_statement();
  } else if (next_token->type_ == TokenType::KEYWORD &&
             (next_token->literal_ == "for" ||
              next_token->literal_ == "do"  ||
              next_token->literal_ == "while")) {
    return parse_iteration_statement();
  }
  return parse_expression_statement();
}

// COMPOUND_STATEMENT => '{' (STATEMENT)* '}'
ASTNodePtr ParserImpl::parse_compound_statement() {
  lexer_->expect_next("{");
  std::vector<ASTNodePtr> children;
  while (true) {
    TokenPtr next_token = lexer_->peek();
    if (next_token->type_ == TokenType::RIGHT_BRACE) {
      lexer_->expect_next("}");
      break;
    }
    ASTNodePtr next_node = nullptr;
    if (next_token->type_ == TokenType::TYPE_SPECIFER) {
      next_node = parse_declaration();
    } else {
      next_node = parse_statement();
    }
    if (next_node->kind_ != ASTNodeKind::EMPTY) {
      children.emplace_back(std::move(next_node));
    }
  }
  return ASTNode::compound_statement(children);
}

// EXPRESSION_STATEMENT => (EXPRESSION)? ';'
ASTNodePtr ParserImpl::parse_expression_statement() {
  TokenPtr next_token = lexer_->peek();
  if (next_token->type_ != TokenType::SEMICOLON) {
    ASTNodePtr next_node = parse_assignment();
    lexer_->expect_next(";");
    return next_node;
  }
  lexer_->expect_next(";");
  return ASTNode::empty();
}

// ITERATION_STATEMENT =>  for '(' EXPRESSION? '; EXPRESSION? ';' EXPRESSION? ')' STATEMENT
//                       | do STATEMENT while '(' EXPRESSION ')' ';'
//                       | while '(' EXPRESSION ')' STATEMENT
ASTNodePtr ParserImpl::parse_iteration_statement() {
  auto next_token = lexer_->peek();
  ASTNodePtr init_expression = nullptr, condition_expression = nullptr, increment_expession = nullptr, statement = nullptr;
  if (next_token->literal_ == "for") {
    lexer_->expect_next("for");
    lexer_->expect_next("(");
      init_expression = lexer_->peek()->type_ == TokenType::SEMICOLON 
                                                 ? ASTNode::empty() 
                                                 : parse_expression();
    lexer_->expect_next(";");
    next_token = lexer_->peek();
      condition_expression = lexer_->peek()->type_ == TokenType::SEMICOLON 
                                                      ? ASTNode::empty() 
                                                      : parse_expression();
    lexer_->expect_next(";");
    next_token = lexer_->peek();
      increment_expession = lexer_->peek()->type_ == TokenType::RIGHT_PARENTHESE 
                                                     ? ASTNode::empty() 
                                                     : parse_expression();
    lexer_->expect_next(")");
    statement = parse_statement();
    return ASTNode::for_loop(std::move(init_expression),
                             std::move(condition_expression),
                             std::move(increment_expession),
                             std::move(statement));
  } else if (next_token->literal_ == "do") {
    lexer_->expect_next("do");
      statement = parse_statement();
    lexer_->expect_next("while");
    lexer_->expect_next("(");
      condition_expression = parse_expression();
    lexer_->expect_next(")");
    lexer_->expect_next(";");
    return ASTNode::do_while(std::move(statement), std::move(condition_expression));
  } else {
    lexer_->expect_next("while");
    lexer_->expect_next("(");
      condition_expression = parse_expression();
    lexer_->expect_next(")");
      statement = parse_statement();
    return ASTNode::while_loop(std::move(condition_expression), std::move(statement));
  }
}

// SELECT_STATEMENT =>  if '(' EXPRESSION ')' STATEMENT
//                    | if '(' EXPRESSION ')' STATEMENT else STATEMENT
ASTNodePtr ParserImpl::parse_select_statement() {
  lexer_->expect_next("if");
  lexer_->expect_next("(");
  auto expression = parse_expression();
  lexer_->expect_next(")");
  auto if_statement = parse_statement();
  auto next_token = lexer_->peek();
  ASTNodePtr else_statement = nullptr;
  if (next_token->type_ == TokenType::KEYWORD &&
      next_token->literal_ == "else") {
    lexer_->expect_next("else");
    else_statement = parse_statement();
  }
  return ASTNode::if_else(expression, if_statement, else_statement);
}

// EXPRESSION => ASSIGNMENT
ASTNodePtr ParserImpl::parse_expression() {
  return parse_assignment();
}

// ASSIGNMENT => EQUALITY ('=' EQUALITY)*
ASTNodePtr ParserImpl::parse_assignment() {
  ASTNodePtr lhs = parse_equality();
  TokenPtr next_token = lexer_->peek();
  bool is_assign = next_token->type_ == TokenType::ASSIGN;
  if (!is_assign) { return lhs; }
  next_token = lexer_->next();
  return ASTNode::assign(lhs, parse_assignment());
}

// EQUALITY => RELATION ('==' RELATION
//                     | '!=' RElATION)
ASTNodePtr ParserImpl::parse_equality() {
  ASTNodePtr lhs = parse_relation();
  while (true) {
    TokenPtr next_token = lexer_->peek();
    bool is_equal = next_token->type_         == TokenType::EQUAL,
         is_not_equal = next_token->type_     == TokenType::NOT_EQUAL;
    if (!is_equal && !is_not_equal) { return lhs; }
    next_token = lexer_->next();
    ASTNodePtr rhs = parse_relation();
    if (is_equal) {
      lhs = ASTNode::equal(lhs, rhs);
    } else {
      lhs = ASTNode::not_equal(lhs, rhs);
    }
  }
}

// RELATION => ADD_AND_SUB ('==' ADD_AND_SUB 
//                        | '!=' ADD_AND_SUB 
//                        | '<' ADD_AND_SUB 
//                        | '<=' ADD_AND_SUB)*
ASTNodePtr ParserImpl::parse_relation() {
  ASTNodePtr lhs = parse_add_and_subtraction();
  while (true) {
    TokenPtr next_token = lexer_->peek();
    bool is_less_than = next_token->type_     == TokenType::LEFT_ANGLE,
         is_less_equal = next_token->type_    == TokenType::LESS_EQUAL,
         is_greater_than = next_token->type_  == TokenType::RIGHT_ANGLE,
         is_greater_equal = next_token->type_ == TokenType::GREATER_EQUAL;
    if (!is_less_than && !is_less_equal 
     && !is_greater_than && !is_greater_equal) { return lhs; }
    next_token = lexer_->next();
    ASTNodePtr rhs = parse_add_and_subtraction();
    if (is_less_than) {
      lhs = ASTNode::less_than(lhs, rhs);
    } else if (is_less_equal) {
      lhs = ASTNode::less_equal(lhs, rhs);
    } else if (is_greater_than) {
      lhs = ASTNode::less_than(rhs, lhs);
    } else {
      lhs = ASTNode::less_equal(rhs, lhs);
    }
  }
}

// ADD_AND_SUB => MULTIPLY ('+' MULTIPLY | '-' MULTIPLY)*
ASTNodePtr ParserImpl::parse_add_and_subtraction() {
  ASTNodePtr lhs = parse_multiply_and_division();
  while (true) {
    TokenPtr next_token = lexer_->peek();
    bool is_add = next_token->type_ == TokenType::PLUS,
         is_sub = next_token->type_ == TokenType::HYPHEN;
    if (!is_add && !is_sub) { return lhs; }
    next_token = lexer_->next();
    ASTNodePtr rhs = parse_multiply_and_division();
    lhs = is_add ? ASTNode::add(lhs, rhs) : ASTNode::subtract(lhs, rhs);
  }
}

// MULTIPLY => UNARY ('*' UNARY | '/' UNARY)*
ASTNodePtr ParserImpl::parse_multiply_and_division() {
  ASTNodePtr lhs = parse_unary();
  while (true) {
    TokenPtr next_token = lexer_->peek();
    bool is_mul = next_token->type_ == TokenType::ASTERISK,
         is_div = next_token->type_ == TokenType::SLASH;
    if (!is_mul && !is_div) { return lhs; }
    next_token = lexer_->next();
    ASTNodePtr rhs = parse_unary();
    lhs = is_mul ? ASTNode::multiply(lhs, rhs) : ASTNode::divide(lhs, rhs);
  }
}

// UNARY => ('-' UNARY | '+' UNARY) | PRIMARY
ASTNodePtr ParserImpl::parse_unary() {
  TokenPtr next_token = lexer_->peek();
  if (next_token->type_ == TokenType::PLUS)  {
    next_token = lexer_->next();
    return parse_unary();
  }
  else if (next_token->type_ == TokenType::HYPHEN) {
    next_token = lexer_->next();
    return ASTNode::negtive(parse_unary());
  }
  return parse_primary();
}

// PRIMARY => INTEGER | VARIABLE | (EXPRESSION)
ASTNodePtr ParserImpl::parse_primary() {
  TokenPtr next_token = lexer_->next();
  switch (next_token->type_) {
    case TokenType::IDENTIFIER: {
      return ASTNode::identifier(next_token->literal_);
    }
    case TokenType::INTEGER: {
      return ASTNode::integer(next_token->literal_);
    }
    case TokenType::LEFT_PARENTHESE: {
      ASTNodePtr expression = parse_expression();
      lexer_->expect_next(")");
      return expression;
    }
    default:
      std::stringstream error_msg;
      error_msg << "operator expected, but " << next_token->literal_ << " found";
      throw std::invalid_argument(error_msg.str());
  }
}

std::unique_ptr<Parser> CreateParser(std::istream& input) {
    return std::make_unique<ParserImpl>(input);
}
