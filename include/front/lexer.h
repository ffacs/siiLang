#pragma once
#include <memory>
#include <string>

#include "diagnose.h"

namespace front {

enum class TokenType : uint32_t {
  UNKNOWN          = 0,
  ED               = 1,
  INTEGER          = 2,
  IDENTIFIER       = 3,
  PLUS             = 4,   // +
  HYPHEN           = 5,   // -
  ASTERISK         = 6,   // *
  SLASH            = 7,   // /
  LEFT_PARENTHESE  = 8,   // (
  RIGHT_PARENTHESE = 9,   // )
  SEMICOLON        = 10,  // ;
  EQUAL            = 11,  // ==
  NOT_EQUAL        = 12,  // !=
  LEFT_ANGLE       = 13,  // <
  LESS_EQUAL       = 14,  // <=
  RIGHT_ANGLE      = 15,  // >
  GREATER_EQUAL    = 16,  // >=
  ASSIGN           = 17,  // =
  LEFT_BRACE       = 18,  // {
  RIGHT_BRACE      = 19,  // }
  LEFT_BRACKET     = 20,  // [
  RIGHT_BRACKET    = 21,  // ]
  KEYWORD          = 22,
  TYPE_SPECIFER    = 23,
  COMMA            = 24,  // ,
  BIT_AND          = 25,  // &
};

struct Token;
typedef std::shared_ptr< Token > TokenPtr;

struct Token {
  Token( TokenType type, std::string_view literal, LexInfo lex_info )
      : type_( type )
      , literal_( std::move( literal ) )
      , lex_info_( std::move( lex_info ) ) {}
  TokenType   type_;
  std::string literal_;
  LexInfo     lex_info_;

  std::string     to_string() const;
  static TokenPtr Unknow( std::string_view literal, LexInfo position );
  static TokenPtr Integer( std::string_view literal, LexInfo position );
  static TokenPtr Identifier( std::string_view literal, LexInfo position );
  static TokenPtr Keyword( std::string_view literal, LexInfo position );
  static TokenPtr Type_specifier( std::string_view literal, LexInfo position );
  static TokenPtr Plus( LexInfo position );
  static TokenPtr Hyphen( LexInfo position );
  static TokenPtr Asterisk( LexInfo position );
  static TokenPtr Slash( LexInfo position );
  static TokenPtr Left_parenthese( LexInfo position );
  static TokenPtr Right_parenthese( LexInfo position );
  static TokenPtr Semicolon( LexInfo position );
  static TokenPtr Equal( LexInfo position );
  static TokenPtr Not_equal( LexInfo position );
  static TokenPtr Left_angle( LexInfo position );
  static TokenPtr Less_equal( LexInfo position );
  static TokenPtr Right_angle( LexInfo position );
  static TokenPtr Greater_equal( LexInfo position );
  static TokenPtr Assgin( LexInfo position );
  static TokenPtr Left_brace( LexInfo position );
  static TokenPtr Right_brace( LexInfo position );
  static TokenPtr Comma( LexInfo position );
  static TokenPtr Left_bracket( LexInfo position );
  static TokenPtr Right_bracket( LexInfo position );
  static TokenPtr Bit_and( LexInfo position );
};

class Lexer {
public:
  Lexer() {}

  virtual ~Lexer() {}

  virtual TokenPtr next() = 0;

  virtual TokenPtr peek() = 0;

  virtual bool have_next() = 0;

  virtual LexPosition current_position() = 0;

  virtual LexInfo get_lex_info( LexPosition begin ) = 0;

  virtual void expect_next( const std::string& expect ) = 0;
};

std::unique_ptr< Lexer > CreateLexer( std::string_view   content,
                                      DiagnoseHandlerPtr diagnose_handler );

}  // namespace front
