#pragma once
#include "front/three_address_code.h"
#include "front/AST.h"

class IRGenerator {
public:
  virtual ~IRGenerator() {}
  virtual std::vector<ThreeAddressCodePtr> work() = 0;
};

std::unique_ptr<IRGenerator> CreateIRGenerator(ASTNodePtr abstract_syntax_tree);
