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
  ASTNodePtr parse_statements();
  ASTNodePtr parse_statement();
  ASTNodePtr parse_expression();
  ASTNodePtr parse_relation();
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
  return ASTNode::statements(children);
}

ASTNodePtr ParserImpl::parse_statement() {
  TokenPtr next_token = lexer_->peek();
  if (next_token->type_ != TokenType::SEMICOLON) {
    ASTNodePtr next_node = parse_expression();
    lexer_->expect_next(";");
    return next_node;
  }
  lexer_->expect_next(";");
  return ASTNode::empty();
}

// EXPR => RELATION
ASTNodePtr ParserImpl::parse_expression() {
  return parse_relation();
}

// RELATION => ADD_AND_SUB ('==' ADD_AND_SUB 
//                        | '!=' ADD_AND_SUB 
//                        | '<' ADD_AND_SUB 
//                        | '<=' ADD_AND_SUB)*
ASTNodePtr ParserImpl::parse_relation() {
  ASTNodePtr lhs = parse_add_and_subtraction();
  while (true) {
    TokenPtr next_token = lexer_->peek();
    bool is_equal = next_token->type_         == TokenType::EQUAL,
         is_not_equal = next_token->type_     == TokenType::NOT_EQUAL,
         is_less_than = next_token->type_     == TokenType::LEFT_ANGLE,
         is_less_equal = next_token->type_    == TokenType::LESS_EQUAL,
         is_greater_than = next_token->type_  == TokenType::RIGHT_ANGLE,
         is_greater_equal = next_token->type_ == TokenType::GREATER_EQUAL;
    if (!is_equal && !is_not_equal 
     && !is_less_than && !is_less_equal 
     && !is_greater_than && !is_greater_equal) { return lhs; }
    next_token = lexer_->next();
    ASTNodePtr rhs = parse_add_and_subtraction();
    if (is_equal) {
      lhs = ASTNode::equal(lhs, rhs);
    } else if (is_not_equal) {
      lhs = ASTNode::not_equal(lhs, rhs);
    } else if (is_less_than) {
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
      return ASTNode::variable(std::move(next_token));
    }
    case TokenType::INTEGER: {
      return ASTNode::integer(std::move(next_token));
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
