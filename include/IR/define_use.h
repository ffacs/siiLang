#pragma once

#include "IR/function.h"

namespace SiiIR {

struct DefineUse {
  enum class Kind : uint32_t {
    DEFINE = 0,
    USE = 1
  };
  DefineUse(Kind kind, BasicGroup* group, SiiIRCode* code)
      : kind_(kind), group_(group), code_(code) {}
  Kind kind_;
  BasicGroup* group_;
  SiiIRCode* code_;
};

}