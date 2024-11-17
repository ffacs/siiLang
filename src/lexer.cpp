#include "lexer.h"

std::string Token::to_string() const {
  std::stringstream ss;
  switch (type_) {
    case TokenType::ED:
      ss << "EOF";
      break;
    case TokenType::INTEGER:
      ss << "Integer: " << literal_;
      break;
    case TokenType::VARIABLE:
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
    default:
      throw std::invalid_argument("Unknow type of token");
  }
  return ss.str();
}

static TokenPtr token(TokenType type, std::string_view literal) {
  return std::make_shared<Token>(type, literal);
}

TokenPtr integer(std::string_view literal) {
  return token(TokenType::INTEGER, literal);
}

TokenPtr variable(std::string_view literal) {
  return token(TokenType::VARIABLE, literal);
}

TokenPtr plus() {
  return token(TokenType::PLUS, "+");
}

TokenPtr hyphen() {
  return token(TokenType::HYPHEN, "-");
}

TokenPtr asterisk() {
  return token(TokenType::ASTERISK, "*");
}

TokenPtr slash() {
  return token(TokenType::SLASH, "/");
}

TokenPtr left_parenthese() {
  return token(TokenType::LEFT_PARENTHESE, "(");
}

TokenPtr right_parenthese() {
  return token(TokenType::RIGHT_PARENTHESE, ")");
}

TokenPtr semicolon() {
  return token(TokenType::SEMICOLON, ";");
}

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
        return plus();
      case '-':
        index_++;
        return hyphen();
      case '*':
        index_++;
        return asterisk();
      case '/':
        index_++;
        return slash();
      case '(':
        index_++;
        return left_parenthese();
      case ')':
        index_++;
        return right_parenthese();
      case ';':
        index_++;
        return semicolon();
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
    return  integer(std::string_view(contents_.c_str() + begin, index_ - begin));
  }

  TokenPtr next_variable() {
    size_t begin = index_;
    while (std::isalnum(current_char())) {
      index_++;
    }
    return token(TokenType::VARIABLE, std::string_view(contents_.c_str() + begin, index_ - begin));
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

std::unique_ptr<Lexer> create_lexer(std::istream& input) {
  return std::make_unique<LexerImpl>(input);
}
