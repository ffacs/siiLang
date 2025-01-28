#pragma once
#include "front/AST.h"
#include <iostream>
#include <memory>

namespace front {
class Parser {
public:
  Parser(std::istream &input) : input_(input) {}
  virtual ~Parser() {}
  virtual ASTNodePtr work() = 0;

  // For test purpose
  virtual ASTNodePtr parse_declaration_or_function_definition() = 0;
  virtual ASTNodePtr parse_compound_statement() = 0;

protected:
  std::istream &input_;
};

std::unique_ptr<Parser> CreateParser(std::istream &input);
} // namespace front