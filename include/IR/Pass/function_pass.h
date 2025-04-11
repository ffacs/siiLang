#pragma once
#include "IR/function.h"

namespace SiiIR {
  class FunctionPass {
    virtual void run( FunctionPtr& func ) = 0;
  };

}  // namespace SiiIR
