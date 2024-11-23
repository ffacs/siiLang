#include "three_address_code.h"
#include <sstream>

std::shared_ptr<Address> Address::variable(const std::string& name) {
  return std::make_shared<VariableAddress>(name);
}

std::shared_ptr<Address> Address::constant(const std::string& literal) {
  return std::make_shared<ConstantAddress>(literal);
}

std::shared_ptr<Address> Address::temporary(ThreeAddressCode* src, const std::string& name) {
  return std::make_shared<TemporaryAddress>(src, name);
}

std::string VariableAddress::to_string() const {
  return std::string("%") + name_;
}

std::string ConstantAddress::to_string() const {
  return literal_;
}

std::string TemporaryAddress::to_string() const {
  return std::string("%") + name_;
}

std::string ThreeAddressCode::to_string() const {
  switch (operator_) {
    case TACOperator::MUL:
      return result_->to_string() + " = " + argL_->to_string() + " * " + argR_->to_string() + ";";
    case TACOperator::DIV:
      return result_->to_string() + " = " + argL_->to_string() + " / " + argR_->to_string() + ";";
    case TACOperator::ADD:
      return result_->to_string() + " = " + argL_->to_string() + " + " + argR_->to_string() + ";";
    case TACOperator::SUB:
      return result_->to_string() + " = " + argL_->to_string() + " - " + argR_->to_string() + ";";
    case TACOperator::EQUAL:
      return result_->to_string() + " = " + argL_->to_string() + " == " + argR_->to_string() + ";";
    case TACOperator::NOT_EQUAL:
      return result_->to_string() + " = " + argL_->to_string() + " != " + argR_->to_string() + ";";
    case TACOperator::LESS_THAN:
      return result_->to_string() + " = " + argL_->to_string() + " < " + argR_->to_string() + ";";
    case TACOperator::LESS_EQUAL:
      return result_->to_string() + " = " + argL_->to_string() + " <= " + argR_->to_string() + ";";
    case TACOperator::NEG:
      return result_->to_string() + " = " + "-" + argL_->to_string() + ";";
    default:
      throw std::invalid_argument("Unknow type of TACOperator");
  }
}
