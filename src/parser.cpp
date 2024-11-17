#include "parser.h"
#include "lexer.h"
#include "AST.h"

class ParserImpl :public Parser {
public:
    ParserImpl(std::istream& input, std::ostream& ouput)
      : Parser(input, ouput) {
      lexer_  = create_lexer(input);
    }
    
    ASTNodePtr work() override;
private:
  std::unique_ptr<Lexer> lexer_;
  ASTNodePtr parse_statements();
  ASTNodePtr parse_statement();
  ASTNodePtr parse_expression();
  ASTNodePtr parse_add_and_subtraction();
  ASTNodePtr parse_multiply_and_division();
  ASTNodePtr parse_unary();
  ASTNodePtr parse_primary();
};

ASTNodePtr ParserImpl::work() {
  return parse_statements();
}

ASTNodePtr ParserImpl::parse_statements() {
  std::vector<ASTNodePtr> children;
  while (true) {
    TokenPtr next_token = lexer_->peek();
    if (next_token->type_ == TokenType::ED) { break; }
    ASTNodePtr next_node = parse_statement();
    if (next_node->type_ != ASTNodeType::EMPTY) {
      children.emplace_back(std::move(next_node));
    }
  }
  return astNode(ASTNodeType::STATEMENTS, nullptr, std::move(children));
}

ASTNodePtr ParserImpl::parse_statement() {
  TokenPtr next_token = lexer_->peek();
  if (next_token->type_ != TokenType::SEMICOLON) {
    ASTNodePtr next_node = parse_expression();
    lexer_->expect_next(";");
    return next_node;
  }
  lexer_->expect_next(";");
  return astNode(ASTNodeType::EMPTY, nullptr);
}

// EXPR => ADD_AND_SUB
ASTNodePtr ParserImpl::parse_expression() {
  return parse_add_and_subtraction();
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
    lhs = is_add ? 
            astNode(ASTNodeType::ADD, next_token, {lhs, rhs})
          : astNode(ASTNodeType::SUB, next_token, {lhs, rhs}); 
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
    lhs = is_mul ? 
              astNode(ASTNodeType::MUL, next_token, {lhs, rhs}) 
            : astNode(ASTNodeType::DIV, next_token, {lhs, rhs});
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
    return astNode(ASTNodeType::NEG, next_token, { parse_unary() });
  }
  return parse_primary();
}

// PRIMARY => INTEGER | VARIABLE | (EXPRESSION)
ASTNodePtr ParserImpl::parse_primary() {
  TokenPtr next_token = lexer_->next();
  switch (next_token->type_) {
    case TokenType::VARIABLE: {
      return astNode(ASTNodeType::VARIABLE, next_token);
    }
    case TokenType::INTEGER: {
      return astNode(ASTNodeType::INTEGER, next_token);
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

std::unique_ptr<Parser> create_parser(std::istream& input, std::ostream& ouput) {
    return std::make_unique<ParserImpl>(input, ouput);
}
