#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <memory>

enum class TokenType : uint32_t {
  ED = 0,
  INTEGER = 1,
  VARIABLE = 2,
  PLUS = 3, // +
  HYPHEN = 4, // -
  ASTERISK = 5, // *
  SLASH = 6, // /
  LEFT_PARENTHESE = 7, // (
  RIGHT_PARENTHESE = 8, // )
  SEMICOLON = 9, // ;
};

struct Token {
  Token(TokenType type, std::string_view literal) :
      type_(type),
      literal_(std::move(literal)) {}
  TokenType type_;
  std::string literal_;

  std::string to_string() const;
};

typedef std::shared_ptr<Token> TokenPtr;

TokenPtr integer(std::string_view literal);
TokenPtr variable(std::string_view literal);
TokenPtr plus();
TokenPtr hyphen();
TokenPtr asterisk();
TokenPtr slash();
TokenPtr left_parenthese();
TokenPtr right_parenthese();
TokenPtr semicolon();

class Lexer {
public:
  Lexer() {}

  virtual ~Lexer() {}

  virtual TokenPtr next() = 0;
  
  virtual TokenPtr peek() = 0;
  
  virtual bool have_next() = 0;

  void expect_next(const std::string& expect) {
    TokenPtr next_token = next();
    if (next_token->literal_ != expect) {
      std::stringstream error_msg;
      error_msg << expect << " expected, but " << next_token->literal_ << " found";
      throw std::invalid_argument(error_msg.str());
    }
  }
};

std::unique_ptr<Lexer> create_lexer(std::istream& input);

