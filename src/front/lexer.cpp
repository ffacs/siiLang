#include "front/lexer.h"
#include <set>
#include <sstream>

namespace front {
std::string Token::to_string() const {
  std::stringstream ss;
  switch ( type_ ) {
  case TokenType::ED             : ss << "EOF"; break;
  case TokenType::INTEGER        : ss << "Integer: " << literal_; break;
  case TokenType::IDENTIFIER     : ss << "Variable: " << literal_; break;
  case TokenType::PLUS           : ss << "Plus: " << literal_; break;
  case TokenType::HYPHEN         : ss << "Hyphen: " << literal_; break;
  case TokenType::ASTERISK       : ss << "Asterisk: " << literal_; break;
  case TokenType::SLASH          : ss << "Slash: " << literal_; break;
  case TokenType::LEFT_PARENTHESE: ss << "Left_parenthese: " << literal_; break;
  case TokenType::RIGHT_PARENTHESE:
    ss << "Right_parenthese: " << literal_;
    break;
  case TokenType::SEMICOLON: ss << "Semicolon: " << literal_; break;
  case TokenType::ASSIGN   : ss << "Assign: " << literal_; break;
  default                  : throw std::invalid_argument( "Unknow type of token" );
  }
  return ss.str();
}

static TokenPtr
token( TokenType type, std::string_view literal, LexInfo lex_info ) {
  return std::make_shared< Token >( type, literal, std::move( lex_info ) );
}

TokenPtr Token::Unknow( std::string_view literal, LexInfo lex_info ) {
  return token( TokenType::UNKNOWN, literal, std::move( lex_info ) );
}

TokenPtr Token::Integer( std::string_view literal, LexInfo lex_info ) {
  return token( TokenType::INTEGER, literal, std::move( lex_info ) );
}

TokenPtr Token::Identifier( std::string_view literal, LexInfo lex_info ) {
  return token( TokenType::IDENTIFIER, literal, std::move( lex_info ) );
}

TokenPtr Token::Plus( LexInfo lex_info ) {
  return token( TokenType::PLUS, "+", std::move( lex_info ) );
}

TokenPtr Token::Hyphen( LexInfo lex_info ) {
  return token( TokenType::HYPHEN, "-", std::move( lex_info ) );
}

TokenPtr Token::Asterisk( LexInfo lex_info ) {
  return token( TokenType::ASTERISK, "*", std::move( lex_info ) );
}

TokenPtr Token::Slash( LexInfo lex_info ) {
  return token( TokenType::SLASH, "/", std::move( lex_info ) );
}

TokenPtr Token::Left_parenthese( LexInfo lex_info ) {
  return token( TokenType::LEFT_PARENTHESE, "(", std::move( lex_info ) );
}

TokenPtr Token::Right_parenthese( LexInfo lex_info ) {
  return token( TokenType::RIGHT_PARENTHESE, ")", std::move( lex_info ) );
}

TokenPtr Token::Semicolon( LexInfo lex_info ) {
  return token( TokenType::SEMICOLON, ";", std::move( lex_info ) );
}

TokenPtr Token::Equal( LexInfo lex_info ) {
  return token( TokenType::EQUAL, "==", std::move( lex_info ) );
}

TokenPtr Token::Not_equal( LexInfo lex_info ) {
  return token( TokenType::NOT_EQUAL, "!=", std::move( lex_info ) );
}

TokenPtr Token::Left_angle( LexInfo lex_info ) {
  return token( TokenType::LEFT_ANGLE, "<", std::move( lex_info ) );
}

TokenPtr Token::Less_equal( LexInfo lex_info ) {
  return token( TokenType::LESS_EQUAL, "<=", std::move( lex_info ) );
}

TokenPtr Token::Right_angle( LexInfo lex_info ) {
  return token( TokenType::RIGHT_ANGLE, ">", std::move( lex_info ) );
}

TokenPtr Token::Greater_equal( LexInfo lex_info ) {
  return token( TokenType::GREATER_EQUAL, ">=", std::move( lex_info ) );
}

TokenPtr Token::Assgin( LexInfo lex_info ) {
  return token( TokenType::ASSIGN, "=", std::move( lex_info ) );
}

TokenPtr Token::Left_brace( LexInfo lex_info ) {
  return token( TokenType::LEFT_BRACE, "{", std::move( lex_info ) );
}

TokenPtr Token::Right_brace( LexInfo lex_info ) {
  return token( TokenType::RIGHT_BRACE, "}", std::move( lex_info ) );
}

TokenPtr Token::Keyword( std::string_view literal, LexInfo lex_info ) {
  return token( TokenType::KEYWORD, literal, std::move( lex_info ) );
}

TokenPtr Token::Type_specifier( std::string_view literal, LexInfo lex_info ) {
  return token( TokenType::TYPE_SPECIFER, literal, std::move( lex_info ) );
}

TokenPtr Token::Comma( LexInfo lex_info ) {
  return token( TokenType::COMMA, ",", std::move( lex_info ) );
}

TokenPtr Token::Left_bracket( LexInfo lex_info ) {
  return token( TokenType::LEFT_BRACKET, "[", std::move( lex_info ) );
}

TokenPtr Token::Right_bracket( LexInfo lex_info ) {
  return token( TokenType::RIGHT_BRACKET, "]", std::move( lex_info ) );
}

TokenPtr Token::Bit_and( LexInfo position ) {
  return token( TokenType::BIT_AND, "&", std::move( position ) );
}

static std::set< std::string > KeyWords
    = { "if", "else", "for", "do", "while", "return" };

static std::set< std::string > TypeSpcifiers = {
  "int",
};

class LexerImpl : public Lexer {
public:
  LexerImpl( std::string_view content, DiagnoseHandlerPtr diagnose_handler )
      : index_( 0 )
      , line_( 1 )
      , column_( 1 )
      , contents_( std::move( content ) )
      , diagnose_handler_( std::move( diagnose_handler ) ) {
    next();
  }

  TokenPtr next() override {
    TokenPtr result = std::move( next_token_ );
    next_token_     = next_word();
    return result;
  }

  virtual TokenPtr peek() override { return next_token_; }

  bool have_next() override { return next_token_->type_ != TokenType::ED; }

  LexPosition current_position() override {
    return LexPosition( index_, line_, column_ );
  }

  LexInfo get_lex_info( LexPosition begin ) override {
    return LexInfo( begin, index_ - begin.begin_index_ );
  }

  void expect_next( const std::string& expect ) override {
    TokenPtr next_token = next();
    if ( next_token->literal_ != expect ) {
      diagnose_handler_->mismatch( DiagnoseLevel::kError,
                                   next_token->lex_info_,
                                   expect,
                                   next_token->literal_ );
    }
  }

private:
  void skip_blank() {
    while ( index_ < contents_.size() && std::isspace( contents_[ index_ ] ) ) {
      if ( contents_[ index_ ] == '\n' || contents_[ index_ ] == '\r' ) {
        line_++;
        column_ = 1;
      } else {
        column_++;
      }
      index_++;
    }
  }

  TokenPtr next_punct() {
    TokenPtr    result         = nullptr;
    LexPosition begin_position = current_position();
    switch ( current_char() ) {
    case '+': {
      index_++;
      result = Token::Plus( get_lex_info( begin_position ) );
      break;
    }
    case '-': {
      index_++;
      result = Token::Hyphen( get_lex_info( begin_position ) );
      break;
    }
    case '*': {
      index_++;
      result = Token::Asterisk( get_lex_info( begin_position ) );
      break;
    }
    case '/': {
      index_++;
      result = Token::Slash( get_lex_info( begin_position ) );
      break;
    }
    case '(': {
      index_++;
      result = Token::Left_parenthese( get_lex_info( begin_position ) );
      break;
    }
    case ')': {
      index_++;
      result = Token::Right_parenthese( get_lex_info( begin_position ) );
      break;
    }
    case ';': {
      index_++;
      result = Token::Semicolon( get_lex_info( begin_position ) );
      break;
    }
    case '=': {
      index_++;
      if ( index_ < contents_.size() && current_char() == '=' ) {
        index_++;
        result = Token::Equal( get_lex_info( begin_position ) );
      } else {
        result = Token::Assgin( get_lex_info( begin_position ) );
      }
      break;
    }
    case '!': {
      index_++;
      if ( index_ < contents_.size() && current_char() == '=' ) {
        index_++;
        result = Token::Not_equal( get_lex_info( begin_position ) );
      } else {
        index_++;
        result = Token::Unknow( contents_.substr( index_ - 2, 2 ),
                                get_lex_info( begin_position ) );
      }
      break;
    }
    case '<': {
      index_++;
      if ( index_ < contents_.size() && current_char() == '=' ) {
        index_++;
        result = Token::Less_equal( get_lex_info( begin_position ) );
      } else {
        result = Token::Left_angle( get_lex_info( begin_position ) );
      }
      break;
    }
    case '>': {
      index_++;
      if ( index_ < contents_.size() && current_char() == '=' ) {
        index_++;
        result = Token::Greater_equal( get_lex_info( begin_position ) );
      } else {
        result = Token::Right_angle( get_lex_info( begin_position ) );
      }
      break;
    }
    case '{': {
      index_++;
      result = Token::Left_brace( get_lex_info( begin_position ) );
      break;
    }
    case '}': {
      index_++;
      result = Token::Right_brace( get_lex_info( begin_position ) );
      break;
    }
    case '[': {
      index_++;
      result = Token::Left_bracket( get_lex_info( begin_position ) );
      break;
    }
    case ']': {
      index_++;
      result = Token::Right_bracket( get_lex_info( begin_position ) );
      break;
    }
    case ',': {
      index_++;
      result = Token::Comma( get_lex_info( begin_position ) );
      break;
    }
    case '&': {
      index_++;
      result = Token::Bit_and( get_lex_info( begin_position ) );
      break;
    }
    default: {
      index_++;
      result = Token::Unknow( contents_.substr( index_ - 1, 1 ),
                              get_lex_info( begin_position ) );
    }
    }
    return result;
  }

  TokenPtr next_integer() {
    size_t      begin          = index_;
    LexPosition begin_position = current_position();
    while ( std::isdigit( current_char() ) ) {
      index_++;
      column_++;
    }
    return Token::Integer(
        std::string_view( contents_.data() + begin, index_ - begin ),
        get_lex_info( begin_position ) );
  }

  TokenPtr next_variable() {
    size_t      begin          = index_;
    LexPosition begin_position = current_position();
    while ( std::isalnum( current_char() ) ) {
      index_++;
      column_++;
    }
    std::string literal( contents_.data() + begin, index_ - begin );
    if ( KeyWords.find( literal ) != KeyWords.end() ) {
      return Token::Keyword( literal, get_lex_info( begin_position ) );
    }
    if ( TypeSpcifiers.find( literal ) != TypeSpcifiers.end() ) {
      return Token::Type_specifier( literal, get_lex_info( begin_position ) );
    }
    return Token::Identifier( literal, get_lex_info( begin_position ) );
  }

  char current_char() { return contents_[ index_ ]; }

  TokenPtr next_word() {
    skip_blank();

    LexPosition begin_position = current_position();
    if ( index_ == contents_.size() ) {
      return token(
          TokenType::ED, std::string_view(), get_lex_info( begin_position ) );
    }

    if ( std::ispunct( current_char() ) ) {
      return next_punct();
    }

    if ( std::isdigit( current_char() ) ) {
      return next_integer();
    }

    if ( std::isalpha( current_char() ) ) {
      return next_variable();
    }

    return Token::Unknow( std::string_view( contents_.data() + index_, 1 ),
                          get_lex_info( begin_position ) );
  }

  TokenPtr           next_token_;
  size_t             index_;
  size_t             line_;
  size_t             column_;
  std::string_view   contents_;
  DiagnoseHandlerPtr diagnose_handler_;
};

std::unique_ptr< Lexer > CreateLexer( std::string_view   content,
                                      DiagnoseHandlerPtr diagnose_handler ) {
  return std::make_unique< LexerImpl >( content, diagnose_handler );
}

}  // namespace front
