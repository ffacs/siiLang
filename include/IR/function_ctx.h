#pragma once
#include "IR/type.h"
#include "IR/value.h"

namespace SiiIR {
struct FunctionContext;
typedef std::shared_ptr<FunctionContext> FunctionContextPtr;
struct FunctionContext {
  TypePtr function_type_;
  std::vector<ValuePtr> allocated_values_;

  FunctionContext(TypePtr function_type)
      : function_type_(std::move(function_type)) {}
  LabelPtr allocate_label();
  TemporaryValuePtr allocate_temporary_value(TypePtr type);
  VariableValuePtr allocate_variable_value(TypePtr type);
};

} // namespace SiiIR