#include "front/parser.h"
#include "front/AST.h"
#include "front/lexer.h"

namespace front {
class ParserImpl : public Parser {
public:
  ParserImpl(std::string file_name, std::istream& input)
      : file_name_(file_name)
      , contents_(std::istreambuf_iterator<char>(input),
                  std::istreambuf_iterator<char>())
      , Parser(input) {
    diagnose_handler_ = CreateDiagnoseHandler(file_name, contents_);
    lexer_            = CreateLexer(contents_, diagnose_handler_);
  }
  ASTNodePtr               work() override;
  CompoundStatementNodePtr parse_compound_statement() override;
  ASTNodePtr               parse_declaration_or_function_definition() override;

  ASTNodePtr parse_translation_unit() override;
  ASTNodePtr parse_expression_statement() override;
  ASTNodePtr parse_select_statement() override;
  ASTNodePtr parse_iteration_statement() override;
  ASTNodePtr parse_jump_statement() override;
  ASTNodePtr parse_statement() override;
  ASTNodePtr parse_expression() override;
  ASTNodePtr parse_assignment() override;
  ASTNodePtr parse_relation() override;
  ASTNodePtr parse_equality() override;
  ASTNodePtr parse_add_and_subtraction() override;
  ASTNodePtr parse_multiply_and_division() override;
  ASTNodePtr parse_unary() override;
  ASTNodePtr parse_primary() override;

private:
  std::vector<DeclaratorPtr> parse_parameter_type_list();
  std::vector<DeclaratorPtr> parse_identifier_list();
  TypePtr                    parse_type_specifier();
  void                       parse_pointer(TypeTransformerPtr transformer);
  ASTNodePtr                 parse_constant_expression();
  LiteralNodePtr parse_direct_declatator(TypeTransformerPtr transformer,
                                         bool               expect_identifier);
  void           parse_function_array_suffix(TypeTransformerPtr transformer);
  std::vector<DeclarationStatementNodePtr> parse_declaration_list();
  DeclarationNodePtr          parse_init_declarator(TypePtr base_type);
  LiteralNodePtr              parse_declarator(TypeTransformerPtr transformer,
                                               bool               expect_identifier);
  DeclarationStatementNodePtr parse_declaration();
  DeclarationStatementNodePtr
  parse_declaration(TypePtr base_type, DeclaratorPtr parsed_declarator);

  std::string            file_name_;
  std::string            contents_;
  DiagnoseHandlerPtr     diagnose_handler_;
  std::unique_ptr<Lexer> lexer_;
};

ASTNodePtr ParserImpl::work() { return parse_translation_unit(); }

// TRANSLATION_UINT => COMPOUND_STATEMENT
ASTNodePtr ParserImpl::parse_translation_unit() {
  auto     result     = parse_declaration_or_function_definition();
  TokenPtr next_token = lexer_->next();
  if(next_token->type_ != TokenType::ED) {
    diagnose_handler_->mismatch(DiagnoseLevel::kError,
                                next_token->lex_info_,
                                "EOF",
                                next_token->literal_);
  }
  return result;
}

// DECLARATION_OR_FUNCTION_DEFINATION =>  TYPE_SPECIFIER DECLARATOR
// DECLARATION_LIST COMPOUND_STATEMENT
//                                      | TYPE_SPECIFIER DECLARATOR
//                                      COMPOUND_STATEMENT | TYPE_SPECIFIER {
//                                      INIT_DECLARATOR { ',' INIT_DECLARATOR
//                                      }*
//                                      }? ';' | DECLARATOR DECLARATION_LIST
//                                      COMPOUND_STATEMENT | DECLARATOR
//                                      COMPOUND_STATEMENT
ASTNodePtr ParserImpl::parse_declaration_or_function_definition() {
  ASTNodePtr  result     = nullptr;
  LexPosition begin_pos  = lexer_->current_position();
  auto        next_token = lexer_->peek();
  if(next_token->type_ == TokenType::TYPE_SPECIFER) {
    auto base_type = parse_type_specifier();
    if(lexer_->peek()->type_ == TokenType::SEMICOLON) {
      result = parse_declaration(std::move(base_type), nullptr);
      goto done;
    }
    TypeTransformerPtr type_transformer = std::make_shared<TypeTransformer>();
    auto               identifier = parse_declarator(type_transformer, true);
    auto               final_type = type_transformer->accept(base_type);
    next_token                    = lexer_->peek();
    // Declaration
    if(next_token->type_ == TokenType::COMMA
       || next_token->type_ == TokenType::SEMICOLON
       || next_token->type_ == TokenType::ASSIGN) {
      result = parse_declaration(
          base_type, Declarator::Create(final_type, identifier->literal_));
      goto done;
    }
    // function definition
    auto declaration_list = parse_declaration_list();
    auto function_body    = parse_compound_statement();

    result = ASTNode::Normalize_declaration(ASTNode::Function_declaration(
        Declarator::Create(final_type, identifier->literal_),
        std::move(declaration_list),
        std::move(function_body)));
    goto done;
  } else {
    auto               base_type = Type::DefaultType();
    TypeTransformerPtr type_transformer
        = std::make_shared<TypeTransformer>(TypeTransformer());
    auto identifier       = parse_declarator(type_transformer, true);
    auto final_type       = type_transformer->accept(base_type);
    auto declaration_list = parse_declaration_list();
    auto function_body    = parse_compound_statement();
    auto declarator = Declarator::Create(final_type, identifier->literal_);
    result          = ASTNode::Normalize_declaration(
        ASTNode::Function_declaration(std::move(declarator),
                                      std::move(declaration_list),
                                      std::move(function_body)));
    goto done;
  }
done:
  result->lex_info_ = lexer_->get_lex_info(begin_pos);
  return result;
}

// DECLARATION_LIST => { DECLARATION }*
std::vector<DeclarationStatementNodePtr> ParserImpl::parse_declaration_list() {
  std::vector<DeclarationStatementNodePtr> declarations;
  while(true) {
    auto next_token = lexer_->peek();
    if(next_token->type_ != TokenType::TYPE_SPECIFER) {
      return declarations;
    }
    declarations.emplace_back(parse_declaration());
  }
}

// TYPE_SPECIFIER => type_specifier
TypePtr ParserImpl::parse_type_specifier() {
  auto type_specifier = lexer_->next();
  if(type_specifier->literal_ == "int") {
    return Type::Basic(TypeKind::INT);
  }
  diagnose_handler_->mismatch(DiagnoseLevel::kError,
                              type_specifier->lex_info_,
                              "Type specifier",
                              type_specifier->literal_);
  return nullptr;
}

DeclarationNodePtr ParserImpl::parse_init_declarator(TypePtr base_type) {
  DeclarationNodePtr result      = nullptr;
  LexPosition        begin_pos   = lexer_->current_position();
  TypeTransformerPtr transformer = std::make_shared<TypeTransformer>();
  auto               identifier  = parse_declarator(transformer, true);
  auto               final_type  = transformer->accept(base_type);
  ASTNodePtr         initializer = nullptr;
  if(lexer_->peek()->type_ == TokenType::ASSIGN) {
    lexer_->expect_next("=");
    initializer = parse_assignment();
  }
  result = ASTNode::Normalize_declaration(
      ASTNode::Declaration(Declarator::Create(final_type, identifier->literal_),
                           std::move(initializer)));
  result->lex_info_ = lexer_->get_lex_info(begin_pos);
  return result;
}

void ParserImpl::parse_pointer(TypeTransformerPtr transformer) {
  while(true) {
    auto next_token = lexer_->peek();
    if(next_token->type_ != TokenType::ASTERISK) {  //*
      break;
    }
    lexer_->expect_next("*");
    transformer->push(std::make_shared<PointerTypeTransformer>());
  }
}

ASTNodePtr ParserImpl::parse_constant_expression() { return parse_equality(); }

std::vector<DeclaratorPtr> ParserImpl::parse_parameter_type_list() {
  std::vector<DeclaratorPtr> result;
  bool                       expect_comma = false;
  while(true) {
    auto next_token = lexer_->peek();
    if(next_token->type_ == TokenType::RIGHT_PARENTHESE) {
      return result;
    }
    if(expect_comma) {
      lexer_->expect_next(",");
    }
    auto               base_type        = parse_type_specifier();
    TypeTransformerPtr type_transformer = std::make_shared<TypeTransformer>();
    auto               identifier = parse_declarator(type_transformer, false);
    auto               final_type = type_transformer->accept(base_type);
    auto declarator = Declarator::Create(final_type, identifier->literal_);
    result.emplace_back(std::move(declarator));
    expect_comma = true;
  }
}

std::vector<DeclaratorPtr> ParserImpl::parse_identifier_list() {
  std::vector<DeclaratorPtr> result;
  bool                       expect_comma = false;
  while(true) {
    auto next_token = lexer_->peek();
    if(next_token->type_ == TokenType::RIGHT_PARENTHESE) {
      return result;
    }
    if(expect_comma) {
      lexer_->expect_next(",");
    }
    next_token = lexer_->next();
    if(next_token->type_ != TokenType::IDENTIFIER) {
      diagnose_handler_->mismatch(DiagnoseLevel::kError,
                                  next_token->lex_info_,
                                  "Identifier",
                                  next_token->literal_);
    }
    auto declarator = Declarator::Create(nullptr, next_token->literal_);
    result.emplace_back(std::move(declarator));
    expect_comma = true;
  }
}

// FUNCTION_ARRAY_SUFFIX =>   EMMPTY
//                          | '[' CONSTANT_EXPRESSION ']'
//                          | '(' ')'
//                          | '( PARAMETER_TYPE_LIST ')'
//                          | '( IDENTIFIER_LIST ')'
void ParserImpl::parse_function_array_suffix(TypeTransformerPtr transformer) {
  auto next_token = lexer_->peek();
  if(next_token->type_ == TokenType::LEFT_PARENTHESE) {  // (
    lexer_->expect_next("(");
    next_token = lexer_->peek();
    std::vector<DeclaratorPtr> parameter_list;
    if(next_token->type_ == TokenType::IDENTIFIER) {
      parameter_list = parse_identifier_list();
    } else if(next_token->type_ == TokenType::TYPE_SPECIFER) {
      parameter_list = parse_parameter_type_list();
    }
    lexer_->expect_next(")");
    parse_function_array_suffix(transformer);
    transformer->push(
        std::make_shared<FunctionTypeTransformer>(parameter_list));
  } else if(next_token->type_ == TokenType::LEFT_BRACKET) {  // [
    lexer_->expect_next("[");
    int64_t element_count = ArrayType::ELEMENT_COUNT_UNKOWN;
    if(lexer_->peek()->type_ != TokenType::RIGHT_BRACKET) {
      auto element_count_expr = parse_constant_expression();
      if(element_count_expr->kind_ != ASTNodeKind::INTEGER) {
        diagnose_handler_->unexpect(
            DiagnoseLevel::kError,
            element_count_expr->lex_info_,
            "Element count of array should be a integer.");
      }
      LiteralNode* integer_node
          = static_cast<LiteralNode*>(element_count_expr.get());
      element_count = std::stoll(integer_node->literal_);
      if(element_count < 0) {
        diagnose_handler_->unexpect(DiagnoseLevel::kError,
                                    element_count_expr->lex_info_,
                                    "declared an array with a negative size");
      }
    }
    lexer_->expect_next("]");
    parse_function_array_suffix(transformer);
    transformer->push(std::make_shared<ArrayTypeTransformer>(element_count));
  }
}

// DIRECT_DECLATATOR => {VARIABLE | '(' DECLARATOR ')'}? FUNCTION_ARRAY_SUFFIX
LiteralNodePtr
ParserImpl::parse_direct_declatator(TypeTransformerPtr transformer,
                                    bool               expect_identifier) {
  LiteralNodePtr result     = ASTNode::Identifier("");
  LexPosition    begin_pos  = lexer_->current_position();
  auto           next_token = lexer_->peek();
  if(next_token->type_ == TokenType::IDENTIFIER) {
    next_token = lexer_->next();
    result     = ASTNode::Identifier(next_token->literal_);
    parse_function_array_suffix(transformer);
  } else if(next_token->type_ == TokenType::LEFT_PARENTHESE) {
    lexer_->expect_next("(");
    TypeTransformerPtr sub_transformer = std::make_shared<TypeTransformer>();
    result = parse_declarator(sub_transformer, expect_identifier);
    lexer_->expect_next(")");
    parse_function_array_suffix(transformer);
    transformer->push(sub_transformer);
  } else if(expect_identifier) {
    diagnose_handler_->mismatch(DiagnoseLevel::kError,
                                next_token->lex_info_,
                                "Identifier or (",
                                next_token->literal_);
  }

  result->lex_info_ = lexer_->get_lex_info(begin_pos);
  return result;
}

// DECLARATOR => POINTER? DIRECT_DECLATATOR
LiteralNodePtr ParserImpl::parse_declarator(TypeTransformerPtr transformer,
                                            bool expect_identifier) {
  LiteralNodePtr result    = nullptr;
  LexPosition    begin_pos = lexer_->current_position();
  parse_pointer(transformer);
  result            = parse_direct_declatator(transformer, expect_identifier);
  result->lex_info_ = lexer_->get_lex_info(begin_pos);
  return result;
}

// DECLARATION => TYPE_SPECIFIER {INIT_DECLARATOR {',' INIT_DECLARATOR}*}? ';'
DeclarationStatementNodePtr
ParserImpl::parse_declaration(TypePtr       base_type,
                              DeclaratorPtr parsed_declarator) {
  DeclarationStatementNodePtr result    = nullptr;
  LexPosition                 begin_pos = lexer_->current_position();
  if(base_type == nullptr) {
    base_type = parse_type_specifier();
  }
  std::vector<DeclarationNodePtr> declaration_list;
  if(parsed_declarator != nullptr) {
    ASTNodePtr initializer = nullptr;
    auto       next_token  = lexer_->peek();
    if(next_token->type_ == TokenType::ASSIGN) {
      lexer_->expect_next("=");
      initializer = parse_assignment();
      next_token  = lexer_->peek();
      if(next_token->type_ == TokenType::COMMA) {
        lexer_->expect_next(",");
      }
    } else if(next_token->type_ == TokenType::COMMA) {
      lexer_->expect_next(",");
    }
    declaration_list.emplace_back(ASTNode::Normalize_declaration(
        ASTNode::Declaration(parsed_declarator, std::move(initializer))));
  }
  while(true) {
    auto next_token = lexer_->peek();
    if(next_token->type_ == TokenType::SEMICOLON) {
      lexer_->expect_next(";");
      result = ASTNode::Declaration_statement(std::move(declaration_list));
      goto done;
    }
    declaration_list.emplace_back(parse_init_declarator(base_type));
    next_token = lexer_->peek();
    if(next_token->type_ == TokenType::SEMICOLON) {
      lexer_->expect_next(";");
      result = ASTNode::Declaration_statement(std::move(declaration_list));
      goto done;
    }
    lexer_->expect_next(",");
  }
done:
  result->lex_info_ = lexer_->get_lex_info(begin_pos);
  return result;
}

DeclarationStatementNodePtr ParserImpl::parse_declaration() {
  return parse_declaration(nullptr, nullptr);
}

// STATEMENT =>  COMPOUND_STATEMENT
//             | EXPRESSION_STATEMENT
//             | SELECT_STATEMENT
//             | ITERATION_STATEMENT
//             | JUMP_STATEMENT
ASTNodePtr ParserImpl::parse_statement() {
  ASTNodePtr  result     = nullptr;
  LexPosition begin_pos  = lexer_->current_position();
  TokenPtr    next_token = lexer_->peek();
  if(next_token->type_ == TokenType::LEFT_BRACE) {
    result = parse_compound_statement();
    goto done;
  } else if(next_token->type_ == TokenType::KEYWORD
            && next_token->literal_ == "if") {
    result = parse_select_statement();
    goto done;
  } else if(next_token->type_ == TokenType::KEYWORD
            && (next_token->literal_ == "for" || next_token->literal_ == "do"
                || next_token->literal_ == "while")) {
    result = parse_iteration_statement();
    goto done;
  } else if(next_token->type_ == TokenType::KEYWORD
            && next_token->literal_ == "return") {
    result = parse_jump_statement();
    goto done;
  }
  result = parse_expression_statement();
done:
  result->lex_info_ = lexer_->get_lex_info(begin_pos);
  return result;
}

// COMPOUND_STATEMENT => '{' (STATEMENT)* '}'
CompoundStatementNodePtr ParserImpl::parse_compound_statement() {
  CompoundStatementNodePtr result    = nullptr;
  LexPosition              begin_pos = lexer_->current_position();
  lexer_->expect_next("{");
  std::vector<ASTNodePtr> children;
  while(true) {
    TokenPtr next_token = lexer_->peek();
    if(next_token->type_ == TokenType::RIGHT_BRACE) {
      lexer_->expect_next("}");
      break;
    }
    ASTNodePtr next_node = nullptr;
    if(next_token->type_ == TokenType::TYPE_SPECIFER) {
      next_node = parse_declaration();
    } else {
      next_node = parse_statement();
    }
    if(next_node->kind_ != ASTNodeKind::EMPTY) {
      children.emplace_back(std::move(next_node));
    }
  }
  result = ASTNode::Compound_statement(children);
done:
  result->lex_info_ = lexer_->get_lex_info(begin_pos);
  return result;
}

// EXPRESSION_STATEMENT => (EXPRESSION)? ';'
ASTNodePtr ParserImpl::parse_expression_statement() {
  ASTNodePtr  result     = nullptr;
  LexPosition begin_pos  = lexer_->current_position();
  TokenPtr    next_token = lexer_->peek();
  if(next_token->type_ != TokenType::SEMICOLON) {
    ASTNodePtr next_node = parse_assignment();
    lexer_->expect_next(";");
    result = next_node;
    goto done;
  }
  lexer_->expect_next(";");
  result = ASTNode::empty();
done:
  result->lex_info_ = lexer_->get_lex_info(begin_pos);
  return result;
}

// ITERATION_STATEMENT =>  for '(' EXPRESSION? '; EXPRESSION? ';' EXPRESSION?
// ')' STATEMENT
//                       | do STATEMENT while '(' EXPRESSION ')' ';'
//                       | while '(' EXPRESSION ')' STATEMENT
ASTNodePtr ParserImpl::parse_iteration_statement() {
  ASTNodePtr  result          = nullptr;
  LexPosition begin_pos       = lexer_->current_position();
  auto        next_token      = lexer_->peek();
  ASTNodePtr  init_expression = nullptr, condition_expression = nullptr,
             increment_expession = nullptr, statement = nullptr;
  if(next_token->literal_ == "for") {
    lexer_->expect_next("for");
    lexer_->expect_next("(");
    init_expression = lexer_->peek()->type_ == TokenType::SEMICOLON
                          ? ASTNode::empty()
                          : parse_expression();
    lexer_->expect_next(";");
    next_token           = lexer_->peek();
    condition_expression = lexer_->peek()->type_ == TokenType::SEMICOLON
                               ? ASTNode::empty()
                               : parse_expression();
    lexer_->expect_next(";");
    next_token          = lexer_->peek();
    increment_expession = lexer_->peek()->type_ == TokenType::RIGHT_PARENTHESE
                              ? ASTNode::empty()
                              : parse_expression();
    lexer_->expect_next(")");
    statement = parse_statement();
    result    = ASTNode::For_loop(std::move(init_expression),
                               std::move(condition_expression),
                               std::move(increment_expession),
                               std::move(statement));
    goto done;
  } else if(next_token->literal_ == "do") {
    lexer_->expect_next("do");
    statement = parse_statement();
    lexer_->expect_next("while");
    lexer_->expect_next("(");
    condition_expression = parse_expression();
    lexer_->expect_next(")");
    lexer_->expect_next(";");
    result = ASTNode::Do_while(std::move(statement),
                               std::move(condition_expression));
    goto done;
  } else {
    lexer_->expect_next("while");
    lexer_->expect_next("(");
    condition_expression = parse_expression();
    lexer_->expect_next(")");
    statement = parse_statement();
    result    = ASTNode::While_loop(std::move(condition_expression),
                                 std::move(statement));
    goto done;
  }
done:
  result->lex_info_ = lexer_->get_lex_info(begin_pos);
  return result;
}

ASTNodePtr ParserImpl::parse_jump_statement() {
  ASTNodePtr  result     = nullptr;
  LexPosition begin_pos  = lexer_->current_position();
  TokenPtr    next_token = lexer_->peek();
  if(next_token->literal_ == "return") {
    lexer_->expect_next("return");
    result = ASTNode::Return(parse_expression());
    lexer_->expect_next(";");
    goto done;
  }
done:
  result->lex_info_ = lexer_->get_lex_info(begin_pos);
  return result;
}

// SELECT_STATEMENT =>  if '(' EXPRESSION ')' STATEMENT
//                    | if '(' EXPRESSION ')' STATEMENT else STATEMENT
ASTNodePtr ParserImpl::parse_select_statement() {
  ASTNodePtr  result    = nullptr;
  LexPosition begin_pos = lexer_->current_position();
  lexer_->expect_next("if");
  lexer_->expect_next("(");
  auto expression = parse_expression();
  lexer_->expect_next(")");
  auto       if_statement   = parse_statement();
  auto       next_token     = lexer_->peek();
  ASTNodePtr else_statement = nullptr;
  if(next_token->type_ == TokenType::KEYWORD
     && next_token->literal_ == "else") {
    lexer_->expect_next("else");
    else_statement = parse_statement();
  }
  result = ASTNode::If_else(expression, if_statement, else_statement);
done:
  result->lex_info_ = lexer_->get_lex_info(begin_pos);
  return result;
}

// EXPRESSION => ASSIGNMENT
ASTNodePtr ParserImpl::parse_expression() { return parse_assignment(); }

// ASSIGNMENT => EQUALITY ('=' EQUALITY)*
ASTNodePtr ParserImpl::parse_assignment() {
  ASTNodePtr  result     = nullptr;
  LexPosition begin_pos  = lexer_->current_position();
  ASTNodePtr  lhs        = parse_equality();
  TokenPtr    next_token = lexer_->peek();
  bool        is_assign  = next_token->type_ == TokenType::ASSIGN;
  if(!is_assign) {
    result = lhs;
    goto done;
  }
  next_token = lexer_->next();
  result     = ASTNode::Assign(lhs, parse_assignment());
done:
  result->lex_info_ = lexer_->get_lex_info(begin_pos);
  return result;
}

// EQUALITY => RELATION ('==' RELATION
//                     | '!=' RElATION)
ASTNodePtr ParserImpl::parse_equality() {
  ASTNodePtr  result    = nullptr;
  LexPosition begin_pos = lexer_->current_position();
  ASTNodePtr  lhs       = parse_relation();
  while(true) {
    TokenPtr next_token = lexer_->peek();
    bool     is_equal   = next_token->type_ == TokenType::EQUAL,
         is_not_equal   = next_token->type_ == TokenType::NOT_EQUAL;
    if(!is_equal && !is_not_equal) {
      result = lhs;
      goto done;
    }
    next_token     = lexer_->next();
    ASTNodePtr rhs = parse_relation();
    if(is_equal) {
      lhs = ASTNode::Equal(lhs, rhs);
    } else {
      lhs = ASTNode::Not_equal(lhs, rhs);
    }
  }
done:
  result->lex_info_ = lexer_->get_lex_info(begin_pos);
  return result;
}

// RELATION => ADD_AND_SUB ('==' ADD_AND_SUB
//                        | '!=' ADD_AND_SUB
//                        | '<' ADD_AND_SUB
//                        | '<=' ADD_AND_SUB)*
ASTNodePtr ParserImpl::parse_relation() {
  ASTNodePtr  result    = nullptr;
  LexPosition begin_pos = lexer_->current_position();
  ASTNodePtr  lhs       = parse_add_and_subtraction();
  while(true) {
    TokenPtr next_token   = lexer_->peek();
    bool     is_less_than = next_token->type_ == TokenType::LEFT_ANGLE,
         is_less_equal    = next_token->type_ == TokenType::LESS_EQUAL,
         is_greater_than  = next_token->type_ == TokenType::RIGHT_ANGLE,
         is_greater_equal = next_token->type_ == TokenType::GREATER_EQUAL;
    if(!is_less_than && !is_less_equal && !is_greater_than
       && !is_greater_equal) {
      result = lhs;
      goto done;
    }
    next_token     = lexer_->next();
    ASTNodePtr rhs = parse_add_and_subtraction();
    if(is_less_than) {
      lhs = ASTNode::Less_than(lhs, rhs);
    } else if(is_less_equal) {
      lhs = ASTNode::Less_equal(lhs, rhs);
    } else if(is_greater_than) {
      lhs = ASTNode::Less_than(rhs, lhs);
    } else {
      lhs = ASTNode::Less_equal(rhs, lhs);
    }
  }
done:
  result->lex_info_ = lexer_->get_lex_info(begin_pos);
  return result;
}

// ADD_AND_SUB => MULTIPLY ('+' MULTIPLY | '-' MULTIPLY)*
ASTNodePtr ParserImpl::parse_add_and_subtraction() {
  ASTNodePtr  result    = nullptr;
  LexPosition begin_pos = lexer_->current_position();
  ASTNodePtr  lhs       = parse_multiply_and_division();
  while(true) {
    TokenPtr next_token = lexer_->peek();
    bool     is_add     = next_token->type_ == TokenType::PLUS,
         is_sub         = next_token->type_ == TokenType::HYPHEN;
    if(!is_add && !is_sub) {
      result = lhs;
      goto done;
    }
    next_token     = lexer_->next();
    ASTNodePtr rhs = parse_multiply_and_division();
    lhs = is_add ? ASTNode::Add(lhs, rhs) : ASTNode::Subtract(lhs, rhs);
  }
done:
  result->lex_info_ = lexer_->get_lex_info(begin_pos);
  return result;
}

// MULTIPLY => UNARY ('*' UNARY | '/' UNARY)*
ASTNodePtr ParserImpl::parse_multiply_and_division() {
  ASTNodePtr  result    = nullptr;
  LexPosition begin_pos = lexer_->current_position();
  ASTNodePtr  lhs       = parse_unary();
  while(true) {
    TokenPtr next_token = lexer_->peek();
    bool     is_mul     = next_token->type_ == TokenType::ASTERISK,
         is_div         = next_token->type_ == TokenType::SLASH;
    if(!is_mul && !is_div) {
      result = lhs;
      goto done;
    }
    next_token     = lexer_->next();
    ASTNodePtr rhs = parse_unary();
    lhs = is_mul ? ASTNode::Multiply(lhs, rhs) : ASTNode::Divide(lhs, rhs);
  }
done:
  result->lex_info_ = lexer_->get_lex_info(begin_pos);
  return result;
}

// UNARY => ('-' UNARY | '+' UNARY | '&' UNARY) | PRIMARY
ASTNodePtr ParserImpl::parse_unary() {
  ASTNodePtr  result     = nullptr;
  LexPosition begin_pos  = lexer_->current_position();
  TokenPtr    next_token = lexer_->peek();
  if(next_token->type_ == TokenType::PLUS) {
    next_token = lexer_->next();
    result     = parse_unary();
    goto done;
  } else if(next_token->type_ == TokenType::HYPHEN) {
    next_token = lexer_->next();
    result     = ASTNode::Negtive(parse_unary());
    goto done;
  } else if(next_token->type_ == TokenType::BIT_AND) {
    next_token = lexer_->next();
    result     = ASTNode::Get_address(parse_unary());
    goto done;
  } else if(next_token->type_ == TokenType::INC_OPE) {
    next_token = lexer_->next();
    result     = ASTNode::Prefix_increase(parse_unary());
    goto done;
  } else if(next_token->type_ == TokenType::DEC_OPE) {
    next_token = lexer_->next();
    result     = ASTNode::Prefix_decrease(parse_unary());
    goto done;
  }
  result = parse_primary();
done:
  result->lex_info_ = lexer_->get_lex_info(begin_pos);
  return result;
}

// PRIMARY => INTEGER | VARIABLE | (EXPRESSION)
ASTNodePtr ParserImpl::parse_primary() {
  ASTNodePtr  result     = nullptr;
  LexPosition begin_pos  = lexer_->current_position();
  TokenPtr    next_token = lexer_->next();
  switch(next_token->type_) {
  case TokenType::IDENTIFIER: {
    result = ASTNode::Identifier(next_token->literal_);
    goto done;
  }
  case TokenType::INTEGER: {
    result = ASTNode::Integer(next_token->literal_);
    goto done;
  }
  case TokenType::LEFT_PARENTHESE: {
    ASTNodePtr expression = parse_expression();
    lexer_->expect_next(")");
    result = expression;
    goto done;
  }
  default:
    diagnose_handler_->mismatch(DiagnoseLevel::kError,
                                next_token->lex_info_,
                                "operator",
                                next_token->literal_);
    return nullptr;
  }
done:
  result->lex_info_ = lexer_->get_lex_info(begin_pos);
  return result;
}

std::unique_ptr<Parser> CreateParser(std::string   file_name,
                                     std::istream& input) {
  return std::make_unique<ParserImpl>(std::move(file_name), input);
}

}  // namespace front
