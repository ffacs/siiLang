#include "IR/value.h"
#include "IR/IR.h"
#include <sstream>

namespace SiiIR {
VariableValuePtr Value::variable(const std::string &name, TypePtr type, TypePtr allocated_type) {
  return std::make_shared<VariableValue>(name, std::move(type), std::move(allocated_type));
}

ConstantValuePtr Value::constant(const std::string &literal, TypePtr type) {
  return std::make_shared<ConstantValue>(literal, std::move(type));
}

TemporaryValuePtr Value::temporary(SiiIRCode *src, const std::string &name,
                                       TypePtr type) {
  return std::make_shared<TemporaryValue>(src, name, std::move(type));
}

UndefValuePtr Value::undef(TypePtr type) {
  return std::make_shared<UndefValue>(std::move(type));
}

FunctionValuePtr
Value::Function(std::shared_ptr<std::vector<SiiIRCodePtr>> codes,
                  FunctionContextPtr ctx, const std::string &name,
                  TypePtr type) {
  return std::make_shared<FunctionValue>(std::move(codes), std::move(ctx),
                                           name, std::move(type));
}

std::string VariableValue::to_string() const {
  return std::string("%") + name_;
}

std::string ConstantValue::to_string() const { return literal_; }

std::string TemporaryValue::to_string() const {
  return std::string("%") + name_;
}

std::string UndefValue::to_string() const { return std::string("undef"); }

std::string FunctionValue::to_string() const {
  std::stringstream result;
  result << "@" + name_ + ":" + "\n";
  for (size_t i = 0; i < codes_->size(); i++) {
    result << (*codes_)[i]->to_string();
    if (i + 1 != codes_->size()) {
      result << "\n";
    }
  }
  return result.str();
}

std::string Label::to_string() const { return std::string("Label.") + name_; }

} // namespace SiiIR
