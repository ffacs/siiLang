#include "three_address_code.h"
#include <sstream>

VariableAddressPtr Address::variable(const std::string& name) {
  return std::make_shared<VariableAddress>(name);
}

ConstantAddressPtr Address::constant(const std::string& literal) {
  return std::make_shared<ConstantAddress>(literal);
}

TemporaryAddressPtr Address::temporary(ThreeAddressCode* src, const std::string& name) {
  return std::make_shared<TemporaryAddress>(src, name);
}

FunctionAddressPtr Address::function(std::shared_ptr<std::vector<ThreeAddressCodePtr>> codes, const std::string& name) {
  return std::make_shared<FunctionAddress>(std::move(codes), name);
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

std::string Label::to_string() const {
  return std::string("Label.") + name_;
}

std::string ThreeAddressCode::to_string() const {
  std::stringstream result_str;
  for (size_t i = 0; i < lable_list_.size(); i++) {
    auto& label = lable_list_[i];
    result_str << label->to_string() +":\n";
  }
  switch (operator_) {
    case TACOperator::MUL: {
      result_str << "  "<< result_->to_string() + " = " + argL_->to_string() + " * " + argR_->to_string() + ";";
      break;
    }
    case TACOperator::DIV: {
      result_str << "  "<< result_->to_string() + " = " + argL_->to_string() + " / " + argR_->to_string() + ";";
      break;
    }
    case TACOperator::ADD: {
      result_str << "  " << result_->to_string() + " = " + argL_->to_string() + " + " + argR_->to_string() + ";";
      break;
    }
    case TACOperator::SUB: {
      result_str << "  " << result_->to_string() + " = " + argL_->to_string() + " - " + argR_->to_string() + ";";
      break;
    }
    case TACOperator::EQUAL: {
      result_str << "  " << result_->to_string() + " = " + argL_->to_string() + " == " + argR_->to_string() + ";";
      break;
    }
    case TACOperator::NOT_EQUAL: {
      result_str << "  " << result_->to_string() + " = " + argL_->to_string() + " != " + argR_->to_string() + ";";
      break;
    }
    case TACOperator::LESS_THAN: {
      result_str << "  " << result_->to_string() + " = " + argL_->to_string() + " < " + argR_->to_string() + ";";
      break;
    }
    case TACOperator::LESS_EQUAL: {
      result_str << "  " << result_->to_string() + " = " + argL_->to_string() + " <= " + argR_->to_string() + ";";
      break;
    }
    case TACOperator::ASSIGN: {
      result_str << "  " << result_->to_string() + " = " + argL_->to_string() + ";";
      break;
    }
    case TACOperator::NEG: {
      result_str << "  " << result_->to_string() + " = " + "-" + argL_->to_string() + ";";
      break;
    }
    case TACOperator::IF_TRUE_GOTO: {
      result_str << "  " << "if " + argL_->to_string() + " goto " + jump_dest_->to_string() + ";";
      break;
    }
    case TACOperator::IF_FALSE_GOTO: {
      result_str << "  " << "if-false " + argL_->to_string() + " goto " + jump_dest_->to_string() + ";";
      break;
    }
    case TACOperator::GOTO: {
      result_str << "  " << "goto " + jump_dest_->to_string() + ";";
      break;
    }
    case TACOperator::NOPE: {
      result_str << "  " << "nope;"; 
      break;
    }
    case TACOperator::FUNCTION_DEFINITION: {
      result_str << argL_->to_string();
      break;
    }
    case TACOperator::ALLOCA: {
      result_str << "  " << "alloca " + argL_->to_string() + " size " + std::to_string(alloca_bytes_) + ";";
      break;
    }
    default:
      throw std::invalid_argument("Unknow type of TACOperator");
  }
  return result_str.str();
}
