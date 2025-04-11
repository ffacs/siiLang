#include "IR/IR.h"
#include <map>
#include <sstream>

namespace SiiIR {
std::string SiiIRCode::to_string(IDAllocator& id_allocator) const {
  std::stringstream result_str;
  if(label_ != nullptr) {
    result_str << label_->to_string(id_allocator) + ":\n";
  }
  return result_str.str();
}

std::string SiiIRBinaryOperation::to_string(IDAllocator& id_allocator) const {
  auto prefix = SiiIRCode::to_string(id_allocator);

  static std::map<SiiIRCodeKind, std::string> binary_operator_str = {
    { SiiIRCodeKind::MUL,        " * "  },
    { SiiIRCodeKind::DIV,        " / "  },
    { SiiIRCodeKind::ADD,        " + "  },
    { SiiIRCodeKind::SUB,        " - "  },
    { SiiIRCodeKind::EQUAL,      " == " },
    { SiiIRCodeKind::NOT_EQUAL,  " != " },
    { SiiIRCodeKind::LESS_THAN,  " < "  },
    { SiiIRCodeKind::LESS_EQUAL, " <= " },
  };

  auto operator_str_iter = binary_operator_str.find(kind_);
  if(operator_str_iter == binary_operator_str.end()) {
    std::stringstream error_str;
    error_str << "Unknown SiiIRCodeKind: " << static_cast<uint32_t>(kind_);
    throw std::runtime_error(error_str.str());
  }
  auto operator_str = operator_str_iter->second;
  return prefix + "  " + id_allocator.alloc(this) + " = "
         + id_allocator.alloc(lhs_->value_.get()) + operator_str
         + id_allocator.alloc(rhs_->value_.get()) + ";";
}

std::string SiiIRUnaryOperation::to_string(IDAllocator& id_allocator) const {
  auto prefix = SiiIRCode::to_string(id_allocator);

  static std::map<SiiIRCodeKind, std::string> unary_operator_str = {
    { SiiIRCodeKind::NEG, "-" },
  };

  auto operator_str_iter = unary_operator_str.find(kind_);
  if(operator_str_iter == unary_operator_str.end()) {
    std::stringstream error_str;
    error_str << "Unknown SiiIRCodeKind: " << static_cast<uint32_t>(kind_);
    throw std::runtime_error(error_str.str());
  }
  auto operator_str = operator_str_iter->second;

  return prefix + "  " + id_allocator.alloc(this) + " = " + operator_str
         + id_allocator.alloc(operand_->value_.get()) + ";";
}

std::string SiiIRConditionBranch::to_string(IDAllocator& id_allocator) const {
  auto prefix = SiiIRCode::to_string(id_allocator);
  return prefix + "  if " + id_allocator.alloc(condition_->value_.get())
         + " goto " + id_allocator.alloc(true_label_->value_.get()) + " else "
         + id_allocator.alloc(false_label_->value_.get()) + ";";
}

std::string SiiIRGoto::to_string(IDAllocator& id_allocator) const {
  auto prefix = SiiIRCode::to_string(id_allocator);
  return prefix + "  goto " + id_allocator.alloc(dest_label_->value_.get())
         + ";";
}

std::string SiiIRNope::to_string(IDAllocator& id_allocator) const {
  return SiiIRCode::to_string(id_allocator) + "  nope;";
}

std::string
SiiIRFunctionDefinition::to_string(IDAllocator& id_allocator) const {
  return SiiIRCode::to_string(id_allocator)
         + function_->to_string(id_allocator);
}

std::string SiiIRAlloca::to_string(IDAllocator& id_allocator) const {
  return SiiIRCode::to_string(id_allocator) + "  " + id_allocator.alloc(this)
         + " = alloca size " + std::to_string(size_) + ";";
}

std::string SiiIRLoad::to_string(IDAllocator& id_allocator) const {
  return SiiIRCode::to_string(id_allocator) + "  " + id_allocator.alloc(this)
         + " = load " + id_allocator.alloc(src_->value_.get()) + ";";
}

std::string SiiIRStore::to_string(IDAllocator& id_allocator) const {
  return SiiIRCode::to_string(id_allocator) + "  store "
         + id_allocator.alloc(src_->value_.get()) + " to "
         + id_allocator.alloc(dest_->value_.get()) + ";";
}

std::string SiiIRPhi::to_string(IDAllocator& id_allocator) const {
  std::string result = SiiIRCode::to_string(id_allocator) + "  "
                       + id_allocator.alloc(this) + " = phi( ";
  for(size_t i = 0; i < src_list_.size(); ++i) {
    result += id_allocator.alloc(src_list_[i]->value_.get());
    if(i != src_list_.size() - 1) {
      result += ", ";
    }
  }
  result += " );";
  return result;
}

std::string SiiIRReturn::to_string(IDAllocator& id_allocator) const {
  return SiiIRCode::to_string(id_allocator) + "  return "
         + id_allocator.alloc(result_->value_.get()) + ";";
}

}  // namespace SiiIR
