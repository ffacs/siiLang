#pragma once
#include "IR/Pass/function_pass.h"

namespace SiiIR {
class MemoryToRegisterPass : public FunctionPass {
public:
  void run(FunctionPtr& func) override;
};

}  // namespace SiiIR
