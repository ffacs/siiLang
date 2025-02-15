#pragma once
#include "IR/value.h"
#include "IR/type.h"

namespace SiiIR {
struct FunctionContext;
typedef std::shared_ptr<FunctionContext> FunctionContextPtr;
struct FunctionContext {
  std::vector<ValuePtr> allocated_values_;

  LabelPtr allocate_label();
  TemporaryValuePtr allocate_temporary_value(TypePtr type);
  VariableValuePtr allocate_variable_value(TypePtr type);
};

} // namespace SiiIR