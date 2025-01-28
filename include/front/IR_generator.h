#pragma once
#include "IR/IR.h"
#include "front/AST.h"

namespace front {
class IRGenerator {
public:
  virtual ~IRGenerator() {}
  virtual std::vector<SiiIR::SiiIRCodePtr> work() = 0;
};

std::unique_ptr<IRGenerator> CreateIRGenerator(ASTNodePtr abstract_syntax_tree);
} // namespace front
