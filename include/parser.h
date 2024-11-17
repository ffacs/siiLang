#pragma once
#include <memory>
#include <iostream>
#include "AST.h"

class Parser {
public:
  Parser(std::istream& input, std::ostream& ouput) : input_(input), output_(ouput) {}
  virtual ~Parser() {}
  virtual ASTNodePtr work() = 0;

 protected:
  std::istream& input_;
  std::ostream& output_;
};

std::unique_ptr<Parser> create_parser(std::istream& input, std::ostream& ouput);
