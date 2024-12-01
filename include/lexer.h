#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <memory>

enum class TokenType : uint32_t {
  ED                    = 0,
  INTEGER               = 1,
  VARIABLE              = 2,
  PLUS                  = 3, // +
  HYPHEN                = 4, // -
  ASTERISK              = 5, // *
  SLASH                 = 6, // /
  LEFT_PARENTHESE       = 7, // (
  RIGHT_PARENTHESE      = 8, // )
  SEMICOLON             = 9, // ;
  EQUAL                 = 10, // ==
  NOT_EQUAL             = 11, // !=
  LEFT_ANGLE            = 12, // <
  LESS_EQUAL            = 13, // <=
  RIGHT_ANGLE           = 14, // >
  GREATER_EQUAL         = 15, // >=
  ASSIGN                = 16, // =
  LEFT_BRACE            = 17, // {
  RIGHT_BRACE           = 18, // }
  LEFT_BRACKET          = 19, // [
  RIGHT_BRACKET         = 20, // ]
  KEYWORD               = 21,
  TYPE_SPECIFER         = 22,
  COMMA                 = 23, // ,
};

struct Token;
typedef std::shared_ptr<Token> TokenPtr;

struct Token {
  Token(TokenType type, std::string_view literal) :
      type_(type),
      literal_(std::move(literal)) {}
  TokenType type_;
  std::string literal_;

  std::string to_string() const;
  static TokenPtr integer(std::string_view literal);
  static TokenPtr variable(std::string_view literal);
  static TokenPtr keyword(std::string_view literal);
  static TokenPtr type_specifier(std::string_view literal);
  static TokenPtr plus();
  static TokenPtr hyphen();
  static TokenPtr asterisk();
  static TokenPtr slash();
  static TokenPtr left_parenthese();
  static TokenPtr right_parenthese();
  static TokenPtr semicolon();
  static TokenPtr equal();
  static TokenPtr not_equal();
  static TokenPtr left_angle();
  static TokenPtr less_equal();
  static TokenPtr right_angle();
  static TokenPtr greater_equal();
  static TokenPtr assgin();
  static TokenPtr left_brace();
  static TokenPtr right_brace();
  static TokenPtr comma();
  static TokenPtr left_bracket();
  static TokenPtr right_bracket();
};


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

std::unique_ptr<Lexer> CreateLexer(std::istream& input);

