#pragma once
#include "front/AST.h"
#include <iostream>
#include <memory>

namespace front {
class Parser {
public:
  Parser( std::istream& input )
      : input_( input ) {}
  virtual ~Parser() {}
  virtual ASTNodePtr work() = 0;

  // For test purpose
  virtual ASTNodePtr parse_declaration_or_function_definition()  = 0;
  virtual CompoundStatementNodePtr parse_compound_statement()    = 0;
  virtual ASTNodePtr               parse_translation_unit()      = 0;
  virtual ASTNodePtr               parse_expression_statement()  = 0;
  virtual ASTNodePtr               parse_select_statement()      = 0;
  virtual ASTNodePtr               parse_iteration_statement()   = 0;
  virtual ASTNodePtr               parse_jump_statement()        = 0;
  virtual ASTNodePtr               parse_statement()             = 0;
  virtual ASTNodePtr               parse_expression()            = 0;
  virtual ASTNodePtr               parse_assignment()            = 0;
  virtual ASTNodePtr               parse_relation()              = 0;
  virtual ASTNodePtr               parse_equality()              = 0;
  virtual ASTNodePtr               parse_add_and_subtraction()   = 0;
  virtual ASTNodePtr               parse_multiply_and_division() = 0;
  virtual ASTNodePtr               parse_unary()                 = 0;
  virtual ASTNodePtr               parse_primary()               = 0;

protected:
  std::istream& input_;
};

std::unique_ptr< Parser > CreateParser( std::string   file_name,
                                        std::istream& input );
}  // namespace front
