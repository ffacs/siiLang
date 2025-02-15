#include "IR/function_ctx.h"

namespace SiiIR {
TemporaryValuePtr FunctionContext::allocate_temporary_value(TypePtr type) {
  uint32_t index = allocated_values_.size();
  auto new_temporary =
      std::make_shared<TemporaryValue>(nullptr, std::to_string(index), std::move(type));
  allocated_values_.push_back(new_temporary);
  return new_temporary;
}

VariableValuePtr FunctionContext::allocate_variable_value(TypePtr type) {
  uint32_t index = allocated_values_.size();
  auto new_variable = std::make_shared<VariableValue>(
      std::to_string(index), type, Type::Pointer(type));
  allocated_values_.push_back(new_variable);
  return new_variable;
}

LabelPtr FunctionContext::allocate_label() {
  uint32_t index = allocated_values_.size();
  auto new_label = std::make_shared<Label>(nullptr, std::to_string(index));
  allocated_values_.push_back(new_label);
  return new_label;
}

} // namespace SiiIR