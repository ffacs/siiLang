#include "IR/function_ctx.h"

namespace SiiIR {
TemporaryAddressPtr FunctionContext::allocate_temporary_address(TypePtr type) {
  uint32_t index = allocated_addresses_.size();
  auto new_temporary =
      std::make_shared<TemporaryAddress>(nullptr, std::to_string(index), std::move(type));
  allocated_addresses_.push_back(new_temporary);
  return new_temporary;
}

VariableAddressPtr FunctionContext::allocate_variable_address(TypePtr type) {
  uint32_t index = allocated_addresses_.size();
  auto new_variable = std::make_shared<VariableAddress>(std::to_string(index), std::move(type));
  allocated_addresses_.push_back(new_variable);
  return new_variable;
}

LabelPtr FunctionContext::allocate_label() {
  uint32_t index = allocated_addresses_.size();
  auto new_label = std::make_shared<Label>(nullptr, std::to_string(index));
  allocated_addresses_.push_back(new_label);
  return new_label;
}

} // namespace SiiIR