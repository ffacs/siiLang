#include "lexer.h"
#include <set>

std::string Token::to_string() const {
  std::stringstream ss;
  switch (type_) {
    case TokenType::ED:
      ss << "EOF";
      break;
    case TokenType::INTEGER:
      ss << "Integer: " << literal_;
      break;
    case TokenType::IDENTIFIER:
      ss << "Variable: " << literal_;
      break;
    case TokenType::PLUS:
      ss << "Plus: " << literal_;
      break;
    case TokenType::HYPHEN:
      ss << "Hyphen: " << literal_;
      break;
    case TokenType::ASTERISK:
      ss << "Asterisk: " << literal_;
      break;
    case TokenType::SLASH:
      ss << "Slash: " << literal_;
      break;
    case TokenType::LEFT_PARENTHESE:
      ss << "Left_parenthese: " << literal_;
      break;
    case TokenType::RIGHT_PARENTHESE:
      ss << "Right_parenthese: " << literal_;
      break;
    case TokenType::SEMICOLON:
      ss << "Semicolon: " << literal_;
      break;
    case TokenType::ASSIGN:
      ss << "Assign: " << literal_;
      break;
    default:
      throw std::invalid_argument("Unknow type of token");
  }
  return ss.str();
}

static TokenPtr token(TokenType type, std::string_view literal) {
  return std::make_shared<Token>(type, literal);
}

TokenPtr Token::integer(std::string_view literal) {
  return token(TokenType::INTEGER, literal);
}

TokenPtr Token::identifier(std::string_view literal) {
  return token(TokenType::IDENTIFIER, literal);
}

TokenPtr Token::plus() {
  return token(TokenType::PLUS, "+");
}

TokenPtr Token::hyphen() {
  return token(TokenType::HYPHEN, "-");
}

TokenPtr Token::asterisk() {
  return token(TokenType::ASTERISK, "*");
}

TokenPtr Token::slash() {
  return token(TokenType::SLASH, "/");
}

TokenPtr Token::left_parenthese() {
  return token(TokenType::LEFT_PARENTHESE, "(");
}

TokenPtr Token::right_parenthese() {
  return token(TokenType::RIGHT_PARENTHESE, ")");
}

TokenPtr Token::semicolon() {
  return token(TokenType::SEMICOLON, ";");
}

TokenPtr Token::equal() {
  return token(TokenType::EQUAL, "==");
}

TokenPtr Token::not_equal() {
  return token(TokenType::NOT_EQUAL, "!=");
}

TokenPtr Token::left_angle() {
  return token(TokenType::LEFT_ANGLE, "<");
}

TokenPtr Token::less_equal() {
  return token(TokenType::LESS_EQUAL, "<=");
}

TokenPtr Token::right_angle() {
  return token(TokenType::RIGHT_ANGLE, ">");
}

TokenPtr Token::greater_equal() {
  return token(TokenType::GREATER_EQUAL, ">=");
}

TokenPtr Token::assgin() {
  return token(TokenType::ASSIGN, "=");
}

TokenPtr Token::left_brace() {
  return token(TokenType::LEFT_BRACE, "{");
}

TokenPtr Token::right_brace() {
  return token(TokenType::RIGHT_BRACE, "}");
}

TokenPtr Token::keyword(std::string_view literal) {
  return token(TokenType::KEYWORD, literal);
}

TokenPtr Token::type_specifier(std::string_view literal) {
  return token(TokenType::TYPE_SPECIFER, literal);
}

TokenPtr Token::comma() {
  return token(TokenType::COMMA, ",");
}

TokenPtr Token::left_bracket() {
  return token(TokenType::LEFT_BRACKET, "[");
}

TokenPtr Token::right_bracket() {
  return token(TokenType::RIGHT_BRACKET, "]");
}

static std::set<std::string> KeyWords = {
  "if",
  "else",
  "for",
  "do",
  "while"
};

static std::set<std::string> TypeSpcifiers = {
  "int",
};

class LexerImpl : public Lexer {
 public:
  LexerImpl(std::istream& input) : 
    index_(0),
    contents_(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>())
  { next(); }

  TokenPtr next() override {
    TokenPtr result = std::move(next_token_);
    next_token_ = next_word();
    return result;
  }

  virtual TokenPtr peek() override {
    return next_token_;
  }

  bool have_next() override {
    return next_token_->type_ != TokenType::ED;
  }

 private:
  void skip_blank() {
    while(index_ < contents_.size() && std::isspace(contents_[index_])) {
      index_++;
    }
  } 

  TokenPtr next_punct() {
    switch (current_char()) {
      case '+':
        index_++;
        return Token::plus();
      case '-':
        index_++;
        return Token::hyphen();
      case '*':
        index_++;
        return Token::asterisk();
      case '/':
        index_++;
        return Token::slash();
      case '(':
        index_++;
        return Token::left_parenthese();
      case ')':
        index_++;
        return Token::right_parenthese();
      case ';':
        index_++;
        return Token::semicolon();
      case '=':
        index_++;
        if (index_ < contents_.size() && current_char() == '=') {
          index_++;
          return Token::equal();
        }
        return Token::assgin();
      case '!':
        index_++;
        if (index_ < contents_.size() && current_char() == '=') {
          index_++;
          return Token::not_equal();
        } else {
          std::stringstream error_msg;
          error_msg << "Unknow punctuator on parsing:\"!" << current_char() << "\""; 
          throw std::invalid_argument(error_msg.str());
        }
      case '<':
        index_++;
        if (index_ < contents_.size() && current_char() == '=') {
          index_++;
          return Token::less_equal();
        }
        return Token::left_angle();
      case '>':
        index_++;
        if (index_ < contents_.size() && current_char() == '=') {
          index_++;
          return Token::greater_equal();
        }
        return Token::right_angle();
      case '{':
        index_++;
        return Token::left_brace();
      case '}':
        index_++;
        return Token::right_brace();
      case '[':
        index_++;
        return Token::left_bracket();
      case ']':
        index_++;
        return Token::right_bracket();
      case ',':
        index_++;
        return Token::comma();
      default:
        std::stringstream error_msg;
        error_msg << "Unknow punctuator on parsing:\"" << current_char() << "\""; 
        throw std::invalid_argument(error_msg.str());
    }
  }

  TokenPtr next_integer() {
    size_t begin = index_;
    while (std::isdigit(current_char())) {
      index_++;
    }
    return Token::integer(std::string_view(contents_.c_str() + begin, index_ - begin));
  }

  TokenPtr next_variable() {
    size_t begin = index_;
    while (std::isalnum(current_char())) {
      index_++;
    }
    std::string literal(contents_.c_str() + begin, index_ - begin);
    if (KeyWords.find(literal) != KeyWords.end()) {
      return Token::keyword(literal);
    }
    if (TypeSpcifiers.find(literal) != TypeSpcifiers.end()) {
      return Token::type_specifier(literal);
    }
    return Token::identifier(literal);
  }

  char current_char() {
    return contents_[index_];
  }
  
  TokenPtr next_word() {
    skip_blank();
    
    if (index_ == contents_.size()) {
      return token(TokenType::ED, std::string_view());
    }

    if (std::ispunct(current_char())) {
      return next_punct();
    }

    if(std::isdigit(current_char())) {
      return next_integer();
    }

    if (std::isalpha(current_char())) {
      return next_variable();
    }
    
    std::stringstream error_msg;
    error_msg << "Unknow character on parsing:\"" << current_char() << "\""; 
    throw std::invalid_argument(error_msg.str());
  }

  TokenPtr next_token_;
  std::string contents_;
  size_t index_;
};

std::unique_ptr<Lexer> CreateLexer(std::istream& input) {
  return std::make_unique<LexerImpl>(input);
}
