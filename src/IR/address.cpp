#include "IR/address.h"
#include "IR/IR.h"
#include <sstream>

namespace SiiIR {
VariableAddressPtr Address::variable(const std::string &name) {
  return std::make_shared<VariableAddress>(name);
}

ConstantAddressPtr Address::constant(const std::string &literal) {
  return std::make_shared<ConstantAddress>(literal);
}

TemporaryAddressPtr Address::temporary(SiiIRCode *src,
                                       const std::string &name) {
  return std::make_shared<TemporaryAddress>(src, name);
}

FunctionAddressPtr
Address::Function(std::shared_ptr<std::vector<SiiIRCodePtr>> codes,
                  FunctionContextPtr ctx, const std::string &name) {
  return std::make_shared<FunctionAddress>(std::move(codes), ctx, name);
}

std::string VariableAddress::to_string() const {
  return std::string("%") + name_;
}

std::string ConstantAddress::to_string() const { return literal_; }

std::string TemporaryAddress::to_string() const {
  return std::string("%") + name_;
}

std::string FunctionAddress::to_string() const {
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
