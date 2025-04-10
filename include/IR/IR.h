#pragma once
#include <cstdint>
#include <functional>

#include "IR/use.h"
#include "IR/value.h"
#include "utils/list.h"

namespace SiiIR {
enum class SiiIRCodeKind : uint32_t {
  MUL = 0,
  DIV = 1,
  ADD = 2,
  SUB = 3,
  NEG = 4,
  EQUAL = 5,
  NOT_EQUAL = 6,
  LESS_THAN = 7,
  LESS_EQUAL = 8,
  GOTO = 9,
  CONDITION_BRANCH = 11,
  NOPE = 12,
  FUNCTION_DEFINITION = 13,
  ALLOCA = 14,
  LOAD = 15,
  STORE = 16,
  PHI = 17,
  RETURN = 18
};

struct BasicGroup;
struct SiiIRCode;
struct SiiIRBinaryOperation;
struct SiiIRUnaryOperation;
struct SiiIRAssign;
struct SiiIRGoto;
struct SiiIRNope;
struct SiiIRConditionBranch;
struct SiiIRFunctionDefinition;
struct SiiIRAlloca;
struct SiiIRStore;
struct SiiIRLoad;
struct SiiIRPhi;
struct SiiIRReturn;
using SiiIRCodePtr = std::shared_ptr<SiiIRCode>;
using SiiIRBinaryOperationPtr = std::shared_ptr<SiiIRBinaryOperation>;
using SiiIRUnaryOperationPtr = std::shared_ptr<SiiIRUnaryOperation>;
using SiiIRGotoPtr = std::shared_ptr<SiiIRGoto>;
using SiiIRNopePtr = std::shared_ptr<SiiIRNope>;
using SiiIRConditionBranchPtr = std::shared_ptr<SiiIRConditionBranch>;
using SiiIRFunctionDefinitionPtr = std::shared_ptr<SiiIRFunctionDefinition>;
using SiiIRAllocaPtr = std::shared_ptr<SiiIRAlloca>;
using SiiIRStorePtr = std::shared_ptr<SiiIRStore>;
using SiiIRLoadPtr = std::shared_ptr<SiiIRLoad>;
using SiiIRPhiPtr = std::shared_ptr<SiiIRPhi>;
using SiiIRReturnPtr = std::shared_ptr<SiiIRReturn>;
using UseSetter = std::function<void(ValuePtr)>;

struct SiiIRCode : public ListNode<SiiIRCode>, public Value {
  SiiIRCodeKind kind_;
  LabelPtr label_;
  BasicGroup *group_ = nullptr;
  explicit SiiIRCode(SiiIRCodeKind kind, TypePtr type)
      : kind_(kind), Value(ValueKind::INSTRUCTION, std::move(type)) {}

  std::string to_string(IDAllocator &id_allocator) const override;
  virtual ~SiiIRCode() = default;
};

struct SiiIRBinaryOperation : public SiiIRCode {
  SiiIRBinaryOperation(SiiIRCodeKind kind, ValuePtr lhs, ValuePtr rhs,
                       TypePtr type)
      : SiiIRCode(kind, std::move(type)), lhs_(NewUse(this, std::move(lhs))),
        rhs_(NewUse(this, std::move(rhs))) {
    lhs_->value_->users_.push_back(lhs_);
    rhs_->value_->users_.push_back(rhs_);
  }

  ~SiiIRBinaryOperation() {
    lhs_->remove_from_parent();
    rhs_->remove_from_parent();
  }

  template <size_t Idx> UseSetter use_setter() {
    return [this](ValuePtr value) {
      if constexpr (Idx == 0) {
        lhs_->remove_from_parent();
        lhs_ = NewUse(this, std::move(value));
        lhs_->value_->users_.push_back(lhs_);
      } else if constexpr (Idx == 1) {
        rhs_->remove_from_parent();
        rhs_ = NewUse(this, std::move(value));
        rhs_->value_->users_.push_back(rhs_);
      }
    };
  }

  std::string to_string(IDAllocator &id_allocator) const override;
  UsePtr lhs_;
  UsePtr rhs_;
};

struct SiiIRUnaryOperation : public SiiIRCode {
  SiiIRUnaryOperation(SiiIRCodeKind kind, ValuePtr operand)
      : SiiIRCode(kind, operand->type_),
        operand_(NewUse(this, std::move(operand))) {
    operand_->value_->users_.push_back(operand_);
  }

  ~SiiIRUnaryOperation() { operand_->remove_from_parent(); }

  template <size_t Idx> UseSetter use_setter() {
    return [this](ValuePtr value) {
      if constexpr (Idx == 0) {
        operand_->remove_from_parent();
        operand_ = NewUse(this, value);
        operand_->value_->users_.push_back(operand_);
      }
    };
  }

  std::string to_string(IDAllocator &id_allocator) const override;
  UsePtr operand_;
};

struct SiiIRGoto : public SiiIRCode {
  explicit SiiIRGoto(LabelPtr dest)
      : SiiIRCode(SiiIRCodeKind::GOTO, nullptr),
        dest_label_(NewUse(this, std::move(dest))) {
    if (dest_label_->value_) {
      dest_label_->value_->users_.push_back(dest_label_);
    }
  }

  ~SiiIRGoto() {
    if (dest_label_->value_) {
      dest_label_->remove_from_parent();
    }
  }

  template <size_t Idx> UseSetter use_setter() {
    return [this](ValuePtr value) {
      if constexpr (Idx == 0) {
        dest_label_->remove_from_parent();
        dest_label_ = NewUse(this, std::move(value));
        dest_label_->value_->users_.push_back(dest_label_);
      }
    };
  }

  std::string to_string(IDAllocator &id_allocator) const override;
  UsePtr dest_label_;
};

struct SiiIRConditionBranch : public SiiIRCode {
  SiiIRConditionBranch(ValuePtr condition, LabelPtr true_label,
                       LabelPtr false_label)
      : SiiIRCode(SiiIRCodeKind::CONDITION_BRANCH, nullptr),
        condition_(NewUse(this, std::move(condition))),
        true_label_(NewUse(this, std::move(true_label))),
        false_label_(NewUse(this, std::move(false_label))) {
    if (condition_->value_) {
      condition_->value_->users_.push_back(condition_);
    }
    if (true_label_->value_) {
      true_label_->value_->users_.push_back(true_label_);
    }
    if (false_label_->value_) {
      false_label_->value_->users_.push_back(false_label_);
    }
  }

  template <size_t Idx> UseSetter use_setter() {
    return [this](ValuePtr value) {
      if constexpr (Idx == 0) {
        condition_->remove_from_parent();
        condition_ = NewUse(this, value);
        if (value) {
          condition_->value_->users_.push_back(condition_);
        }
      } else if constexpr (Idx == 1) {
        true_label_->remove_from_parent();
        true_label_ = NewUse(this, value);
        if (value) {
          true_label_->value_->users_.push_back(true_label_);
        }
      } else if constexpr (Idx == 2) {
        false_label_->remove_from_parent();
        false_label_ = NewUse(this, value);
        if (value) {
          false_label_->value_->users_.push_back(false_label_);
        }
      }
    };
  }

  ~SiiIRConditionBranch() {
    condition_->remove_from_parent();
    true_label_->remove_from_parent();
    false_label_->remove_from_parent();
  }

  std::string to_string(IDAllocator &id_allocator) const override;
  UsePtr condition_;
  UsePtr true_label_;
  UsePtr false_label_;
};

struct SiiIRNope : public SiiIRCode {
  SiiIRNope() : SiiIRCode(SiiIRCodeKind::NOPE, nullptr) {}
  std::string to_string(IDAllocator &id_allocator) const override;
};

struct SiiIRFunctionDefinition : public SiiIRCode {
  SiiIRFunctionDefinition(FunctionValuePtr function)
      : SiiIRCode(SiiIRCodeKind::FUNCTION_DEFINITION, function->type_),
        function_(std::move(function)) {}

  std::string to_string(IDAllocator &id_allocator) const override;
  FunctionValuePtr function_;
};

struct SiiIRAlloca : public SiiIRCode {
  SiiIRAlloca(uint32_t size, TypePtr type)
      : SiiIRCode(SiiIRCodeKind::ALLOCA, Type::Pointer(type)), size_(size) {}

  std::string to_string(IDAllocator &id_allocator) const override;
  uint32_t size_;
};

struct SiiIRLoad : public SiiIRCode {
  SiiIRLoad(ValuePtr source_address)
      : SiiIRCode(SiiIRCodeKind::LOAD, Type::GetAimType(source_address->type_)),
        src_(NewUse(this, std::move(source_address))) {
    src_->value_->users_.push_back(src_);
  }

  ~SiiIRLoad() override { src_->remove_from_parent(); }

  template <size_t Idx> UseSetter use_setter() {
    return [this](ValuePtr value) {
      if constexpr (Idx == 0) {
        src_->remove_from_parent();
        src_ = NewUse(this, value);
        if (value) {
          src_->value_->users_.push_back(src_);
        }
      }
    };
  }

  std::string to_string(IDAllocator &id_allocator) const override;
  UsePtr src_;
};

struct SiiIRStore : public SiiIRCode {
  SiiIRStore(ValuePtr src, ValuePtr dest)
      : SiiIRCode(SiiIRCodeKind::STORE, nullptr),
        src_(NewUse(this, std::move(src))),
        dest_(NewUse(this, std::move(dest))) {

    src_->value_->users_.push_back(src_);
    dest_->value_->users_.push_back(dest_);
  }

  ~SiiIRStore() override {
    src_->remove_from_parent();
    dest_->remove_from_parent();
  }

  template <size_t Idx> UseSetter use_setter() {
    return [this](ValuePtr value) {
      if constexpr (Idx == 0) {
        src_->remove_from_parent();
        src_ = NewUse(this, value);
        src_->value_->users_.push_back(src_);
      } else if constexpr (Idx == 1) {
        dest_->remove_from_parent();
        dest_ = NewUse(this, value);
        dest_->value_->users_.push_back(dest_);
      }
    };
  }

  std::string to_string(IDAllocator &id_allocator) const override;
  UsePtr src_;
  UsePtr dest_;
};

struct SiiIRPhi : public SiiIRCode {
  SiiIRPhi(ValuePtr variale_address, size_t src_size)
      : SiiIRCode(SiiIRCodeKind::PHI, Type::GetAimType(variale_address->type_)),
        src_list_(src_size, nullptr) {
    for (int i = 0; i < src_size; i++) {
      src_list_[i] = NewUse(this, variale_address);
      src_list_[i]->value_->users_.push_back(src_list_[i]);
    }
  }

  void replace_src(size_t index, ValuePtr new_src) {
    src_list_[index]->remove_from_parent();
    src_list_[index] = NewUse(this, new_src);
    src_list_[index]->value_->users_.push_back(src_list_[index]);
  }

  ~SiiIRPhi() override {}

  std::string to_string(IDAllocator &id_allocator) const override;
  std::vector<UsePtr> src_list_;
};

struct SiiIRReturn : public SiiIRCode {
  SiiIRReturn(ValuePtr value)
      : SiiIRCode(SiiIRCodeKind::RETURN, nullptr),
        result_(NewUse(this, value)) {
    result_->value_->users_.push_back(result_);
  }

  ~SiiIRReturn() override { result_->remove_from_parent(); }

  template <size_t Idx> UseSetter use_setter() {
    return [this](ValuePtr value) {
      if constexpr (Idx == 0) {
        result_->remove_from_parent();
        result_ = NewUse(this, value);
        result_->value_->users_.push_back(result_);
      }
    };
  }

  std::string to_string(IDAllocator &id_allocator) const override;
  UsePtr result_;
};

} // namespace SiiIR
