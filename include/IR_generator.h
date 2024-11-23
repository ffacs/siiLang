#pragma once
#include "three_address_code.h"

class IRGenerator {
 public:
  virtual ~IRGenerator() {}
  virtual std::vector<ThreeAddressCodePtr> work() = 0;
};

std::unique_ptr<IRGenerator> CreateIRGenerator(ASTNodePtr abstract_syntax_tree);
