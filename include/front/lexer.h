#pragma once
#include <iostream>
#include <memory>
#include <string>

#include "diagnose.h"

namespace front {

enum class TokenType : uint32_t {
  UNKNOWN = 0,
  ED = 1,
  INTEGER = 2,
  IDENTIFIER = 3,
  PLUS = 4,             // +
  HYPHEN = 5,           // -
  ASTERISK = 6,         // *
  SLASH = 7,            // /
  LEFT_PARENTHESE = 8,  // (
  RIGHT_PARENTHESE = 9, // )
  SEMICOLON = 10,       // ;
  EQUAL = 11,           // ==
  NOT_EQUAL = 12,       // !=
  LEFT_ANGLE = 13,      // <
  LESS_EQUAL = 14,      // <=
  RIGHT_ANGLE = 15,     // >
  GREATER_EQUAL = 16,   // >=
  ASSIGN = 17,          // =
  LEFT_BRACE = 18,      // {
  RIGHT_BRACE = 19,     // }
  LEFT_BRACKET = 20,    // [
  RIGHT_BRACKET = 21,   // ]
  KEYWORD = 22,
  TYPE_SPECIFER = 23,
  COMMA = 24, // ,
};

struct Token;
typedef std::shared_ptr<Token> TokenPtr;

struct Token {
  Token(TokenType type, std::string_view literal, LexInfo lex_info)
      : type_(type), literal_(std::move(literal)),
        lex_info_(std::move(lex_info)) {}
  TokenType type_;
  std::string literal_;
  LexInfo lex_info_;

  std::string to_string() const;
  static TokenPtr unknow(std::string_view literal, LexInfo position);
  static TokenPtr integer(std::string_view literal, LexInfo position);
  static TokenPtr identifier(std::string_view literal, LexInfo position);
  static TokenPtr keyword(std::string_view literal, LexInfo position);
  static TokenPtr type_specifier(std::string_view literal, LexInfo position);
  static TokenPtr plus(LexInfo position);
  static TokenPtr hyphen(LexInfo position);
  static TokenPtr asterisk(LexInfo position);
  static TokenPtr slash(LexInfo position);
  static TokenPtr left_parenthese(LexInfo position);
  static TokenPtr right_parenthese(LexInfo position);
  static TokenPtr semicolon(LexInfo position);
  static TokenPtr equal(LexInfo position);
  static TokenPtr not_equal(LexInfo position);
  static TokenPtr left_angle(LexInfo position);
  static TokenPtr less_equal(LexInfo position);
  static TokenPtr right_angle(LexInfo position);
  static TokenPtr greater_equal(LexInfo position);
  static TokenPtr assgin(LexInfo position);
  static TokenPtr left_brace(LexInfo position);
  static TokenPtr right_brace(LexInfo position);
  static TokenPtr comma(LexInfo position);
  static TokenPtr left_bracket(LexInfo position);
  static TokenPtr right_bracket(LexInfo position);
};

class Lexer {
public:
  Lexer() {}

  virtual ~Lexer() {}

  virtual TokenPtr next() = 0;

  virtual TokenPtr peek() = 0;

  virtual bool have_next() = 0;

  virtual LexPosition current_position() = 0;

  virtual LexInfo get_lex_info(LexPosition begin) = 0;

  virtual void expect_next(const std::string &expect) = 0;
};

std::unique_ptr<Lexer> CreateLexer(std::string_view content,
                                   DiagnoseHandlerPtr diagnose_handler);

} // namespace front