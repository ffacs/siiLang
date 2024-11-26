#include "parser.h"
#include "lexer.h"
#include "AST.h"

class ParserImpl :public Parser {
public:
    ParserImpl(std::istream& input)
      : Parser(input) {
      lexer_  = CreateLexer(input);
    }
    
    ASTNodePtr work() override;
private:
  std::unique_ptr<Lexer> lexer_;
  ASTNodePtr parse_translation_unit();
  ASTNodePtr parse_compound_statement();
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
};

ASTNodePtr ParserImpl::work() {
  return parse_translation_unit();
}

// TRANSLATION_UINT => COMPOUND_STATEMENT
ASTNodePtr ParserImpl::parse_translation_unit() {
  auto result = parse_compound_statement();
  TokenPtr next_token = lexer_->next();
  if (next_token->type_ != TokenType::ED) {
    throw std::invalid_argument("EOF expected!");
  }
  return result;
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
    ASTNodePtr next_node = parse_statement();
    if (next_node->type_ != ASTNodeType::EMPTY) {
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

//ASSIGNMENT => EQUALITY ('=' EQUALITY)*
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
    case TokenType::VARIABLE: {
      return ASTNode::variable(next_token->literal_);
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
