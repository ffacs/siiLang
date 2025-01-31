#include "IR/function_ctx.h"

namespace SiiIR {
TemporaryAddressPtr FunctionContext::allocate_temporary_address() {
  auto new_temporary = std::make_shared<TemporaryAddress>(
      nullptr, "tmp" + std::to_string(temporary_addresses_.size()));
  temporary_addresses_.emplace_back(new_temporary);
  return new_temporary;
}

VariableAddressPtr FunctionContext::allocate_variable_address() {
  auto new_variable = std::make_shared<VariableAddress>(
      "var" + std::to_string(variable_addresses_.size()));
  variable_addresses_.emplace_back(new_variable);
  return new_variable;
}

void FunctionContext::rename_all_addresses() {
  uint32_t idx = 0;
  for (auto &variable : variable_addresses_) {
    variable->name_ = std::to_string(idx++);
  }
  for (auto &temporary : temporary_addresses_) {
    temporary->name_ = std::to_string(idx++);
  }
}

}