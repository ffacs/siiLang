#pragma once
#include "IR/Pass/function_pass.h"

namespace SiiIR {
class QuitSSAPass : public FunctionPass {
public:
  void run(FunctionPtr& func) override;
};
}