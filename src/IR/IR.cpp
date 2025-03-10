#include "IR/IR.h"
#include <sstream>

namespace SiiIR {
std::string SiiIRCode::to_string() const {
  std::stringstream result_str;
  if (label_ != nullptr) {
    result_str << label_->to_string() + ":\n";
  }
  return result_str.str();
}

std::string SiiIRBinaryOperation::to_string() const {
  auto prefix = SiiIRCode::to_string();
  switch (kind_) {
  case SiiIRCodeKind::MUL: {
    return prefix + "  " + result_->to_string() + " = " + lhs_->to_string() +
           " * " + rhs_->to_string() + ";";
  }
  case SiiIRCodeKind::DIV: {
    return prefix + "  " + result_->to_string() + " = " + lhs_->to_string() +
           " / " + rhs_->to_string() + ";";
  }
  case SiiIRCodeKind::ADD: {
    return prefix + "  " + result_->to_string() + " = " + lhs_->to_string() +
           " + " + rhs_->to_string() + ";";
  }
  case SiiIRCodeKind::SUB: {
    return prefix + "  " + result_->to_string() + " = " + lhs_->to_string() +
           " - " + rhs_->to_string() + ";";
  }
  case SiiIRCodeKind::EQUAL: {
    return prefix + "  " + result_->to_string() + " = " + lhs_->to_string() +
           " == " + rhs_->to_string() + ";";
  }
  case SiiIRCodeKind::NOT_EQUAL: {
    return prefix + "  " + result_->to_string() + " = " + lhs_->to_string() +
           " != " + rhs_->to_string() + ";";
  }
  case SiiIRCodeKind::LESS_THAN: {
    return prefix + "  " + result_->to_string() + " = " + lhs_->to_string() +
           " < " + rhs_->to_string() + ";";
  }
  case SiiIRCodeKind::LESS_EQUAL: {
    return prefix + "  " + result_->to_string() + " = " + lhs_->to_string() +
           " <= " + rhs_->to_string() + ";";
  }
  default: {
    std::stringstream error_str;
    error_str << "Unknown SiiIRCodeKind: " << static_cast<uint32_t>(kind_);
    throw std::runtime_error(error_str.str());
  }
  }
}

std::string SiiIRUnaryOperation::to_string() const {
  auto prefix = SiiIRCode::to_string();
  switch (kind_) {
  case SiiIRCodeKind::NEG: {
    return prefix + "  " + result_->to_string() + " = -" +
           operand_->to_string() + ";";
  }
  default: {
    std::stringstream error_str;
    error_str << "Unknown SiiIRCodeKind: " << static_cast<uint32_t>(kind_);
    throw std::runtime_error(error_str.str());
  }
  }
}

std::string SiiIRConditionBranch::to_string() const {
  auto prefix = SiiIRCode::to_string();
  return prefix + "  if " + condition_->to_string() + " goto " +
         true_label_->to_string() + " else " + false_label_->to_string() + ";";
}

std::string SiiIRGoto::to_string() const {
  auto prefix = SiiIRCode::to_string();
  return prefix + "  goto " + dest_label_->to_string() + ";";
}

std::string SiiIRNope::to_string() const {
  return SiiIRCode::to_string() + "  nope;";
}

std::string SiiIRFunctionDefinition::to_string() const {
  return SiiIRCode::to_string() + function_->to_string();
}

std::string SiiIRAlloca::to_string() const {
  return SiiIRCode::to_string() + "  " + dest_->to_string() +
         " = alloca size " + std::to_string(size_) + ";";
}

std::string SiiIRLoad::to_string() const {
  return SiiIRCode::to_string() + "  " + dest_->to_string() + " = load " +
         src_->to_string() + ";";
}

std::string SiiIRStore::to_string() const {
  return SiiIRCode::to_string() + "  store " + src_->to_string() + " to " +
         dest_->to_string() + ";";
}

std::string SiiIRPhi::to_string() const {
  std::string result = SiiIRCode::to_string() + "  " + dest_->to_string() +
                       " = phi( ";
  for (size_t i = 0; i < src_list_.size(); ++i) {
    result += src_list_[i]->to_string();
    if (i != src_list_.size() - 1) {
      result += ", ";
    }
  }
  result += " );";
  return result;

}

} // namespace SiiIR
