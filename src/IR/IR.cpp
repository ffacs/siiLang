#include "IR/IR.h"
#include <sstream>

namespace SiiIR {
std::string SiiIRCode::to_string() const {
  std::stringstream result_str;
  for (const auto &label : labels_) {
    result_str << label->to_string() + ":\n";
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

std::string SiiIRAssign::to_string() const {
  auto prefix = SiiIRCode::to_string();
  return prefix + "  " + dest_->to_string() + " = " + src_->to_string() + ";";
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

std::string SiiIRIfTrueGoto::to_string() const {
  auto prefix = SiiIRCode::to_string();
  return prefix + "  if " + condition_->to_string() + " goto " +
         dest_label_->to_string() + ";";
}

std::string SiiIRIfFalseGoto::to_string() const {
  auto prefix = SiiIRCode::to_string();
  return prefix + "  if-false " + condition_->to_string() + " goto " +
         dest_label_->to_string() + ";";
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

} // namespace SiiIR
