#pragma once
#include <cstdint>
#include <string>

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

struct SiiIRCode : public ListNode<SiiIRCode> {
  SiiIRCodeKind kind_;
  LabelPtr label_;
  BasicGroup *group_ = nullptr;
  List<Use> uses_;
  explicit SiiIRCode(SiiIRCodeKind kind) : kind_(kind) {}

  virtual std::string to_string() const;
  virtual ~SiiIRCode() = default;
};

struct SiiIRBinaryOperation : public SiiIRCode {
  SiiIRBinaryOperation(SiiIRCodeKind kind, ValuePtr lhs, ValuePtr rhs,
                       ValuePtr result)
      : SiiIRCode(kind), lhs_(NewUse(this, std::move(lhs))),
        rhs_(NewUse(this, std::move(rhs))),
        result_(NewUse(this, std::move(result))) {
    lhs_->value_->users_.push_back(lhs_);
    rhs_->value_->users_.push_back(rhs_);
    result_->value_->users_.push_back(result_);
  }

  ~SiiIRBinaryOperation() {
    lhs_->remove_from_parent();
    rhs_->remove_from_parent();
    result_->remove_from_parent();
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
      } else if constexpr (Idx == 2) {
        result_->remove_from_parent();
        result_ = NewUse(this, std::move(value));
        result_->value_->users_.push_back(result_);
      }
    };
  }

  std::string to_string() const override;
  UsePtr lhs_;
  UsePtr rhs_;
  UsePtr result_;
};

struct SiiIRUnaryOperation : public SiiIRCode {
  SiiIRUnaryOperation(SiiIRCodeKind kind, ValuePtr operand, ValuePtr result)
      : SiiIRCode(kind), operand_(NewUse(this, std::move(operand))),
        result_(NewUse(this, std::move(result))) {
    operand_->value_->users_.push_back(operand_);
    result_->value_->users_.push_back(result_);
  }

  ~SiiIRUnaryOperation() { operand_->remove_from_parent(); }

  template <size_t Idx> UseSetter use_setter() {
    return [this](ValuePtr value) {
      if constexpr (Idx == 0) {
        operand_->remove_from_parent();
        operand_ = NewUse(this, value);
        operand_->value_->users_.push_back(operand_);
      } else if constexpr (Idx == 1) {
        result_->remove_from_parent();
        result_ = NewUse(this, value);
        result_->value_->users_.push_back(result_);
      }
    };
  }

  std::string to_string() const override;
  UsePtr operand_;
  UsePtr result_;
};

struct SiiIRGoto : public SiiIRCode {
  explicit SiiIRGoto(LabelPtr dest)
      : SiiIRCode(SiiIRCodeKind::GOTO),
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

  std::string to_string() const override;
  UsePtr dest_label_;
};

struct SiiIRConditionBranch : public SiiIRCode {
  SiiIRConditionBranch(TemporaryValuePtr condition, LabelPtr true_label,
                       LabelPtr false_label)
      : SiiIRCode(SiiIRCodeKind::CONDITION_BRANCH),
        condition_(NewUse(this, std::move(condition))),
        true_label_(NewUse(this, std::move(true_label))),
        false_label_(NewUse(this, std::move(false_label))) {
    condition_->value_->users_.push_back(condition_);
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
        condition_->value_->users_.push_back(condition_);
      } else if constexpr (Idx == 1) {
        true_label_->remove_from_parent();
        true_label_ = NewUse(this, value);
        true_label_->value_->users_.push_back(true_label_);
      } else if constexpr (Idx == 2) {
        false_label_->remove_from_parent();
        false_label_ = NewUse(this, value);
        false_label_->value_->users_.push_back(false_label_);
      }
    };
  }

  ~SiiIRConditionBranch() {
    condition_->remove_from_parent();
    true_label_->remove_from_parent();
    false_label_->remove_from_parent();
  }

  std::string to_string() const override;
  UsePtr condition_;
  UsePtr true_label_;
  UsePtr false_label_;
};

struct SiiIRNope : public SiiIRCode {
  SiiIRNope() : SiiIRCode(SiiIRCodeKind::NOPE) {}
  std::string to_string() const override;
};

struct SiiIRFunctionDefinition : public SiiIRCode {
  SiiIRFunctionDefinition(FunctionValuePtr function)
      : SiiIRCode(SiiIRCodeKind::FUNCTION_DEFINITION),
        function_(std::move(function)) {}
  std::string to_string() const override;
  FunctionValuePtr function_;
};

struct SiiIRAlloca : public SiiIRCode {
  SiiIRAlloca(VariableValuePtr dest, uint32_t size)
      : SiiIRCode(SiiIRCodeKind::ALLOCA), dest_(NewUse(this, std::move(dest))),
        size_(size) {
    dest_->value_->users_.push_back(dest_);
  }

  ~SiiIRAlloca() override { dest_->remove_from_parent(); }

  template <size_t Idx> UseSetter use_setter() {
    return [this](ValuePtr value) {
      if constexpr (Idx == 0) {
        dest_->remove_from_parent();
        dest_ = NewUse(this, value);
        dest_->value_->users_.push_back(dest_);
      }
    };
  }

  std::string to_string() const override;
  UsePtr dest_;
  uint32_t size_;
};

struct SiiIRLoad : public SiiIRCode {
  SiiIRLoad(VariableValuePtr src, TemporaryValuePtr dest)
      : SiiIRCode(SiiIRCodeKind::LOAD), src_(NewUse(this, std::move(src))),
        dest_(NewUse(this, std::move(dest))) {
    src_->value_->users_.push_back(src_);
    dest_->value_->users_.push_back(dest_);
  }

  ~SiiIRLoad() override {
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

  std::string to_string() const override;
  UsePtr src_;
  UsePtr dest_;
};

struct SiiIRStore : public SiiIRCode {
  SiiIRStore(ValuePtr src, ValuePtr dest)
      : SiiIRCode(SiiIRCodeKind::STORE), src_(NewUse(this, std::move(src))),
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

  std::string to_string() const override;
  UsePtr src_;
  UsePtr dest_;
};

struct SiiIRPhi : public SiiIRCode {
  SiiIRPhi(VariableValuePtr variable, size_t src_size)
      : SiiIRCode(SiiIRCodeKind::PHI), dest_(NewUse(this, variable)),
        src_list_(src_size, nullptr) {
    dest_->value_->users_.push_back(dest_);
    for (int i = 0; i < src_size; i++) {
      src_list_[i] = NewUse(this, dest_->value_);
      dest_->value_->users_.push_back(src_list_[i]);
    }
  }

  template <size_t Idx> UseSetter use_setter() {
    return [this](ValuePtr value) {
      if constexpr (Idx == 0) {
        dest_->remove_from_parent();
        dest_ = NewUse(this, value);
        dest_->value_->users_.push_back(dest_);
      }
    };
  }

  void replace_src(size_t index, ValuePtr new_src) {
    src_list_[index]->remove_from_parent();
    src_list_[index] = NewUse(this, new_src);
    src_list_[index]->value_->users_.push_back(src_list_[index]);
  }

  ~SiiIRPhi() override { dest_->remove_from_parent(); }

  std::string to_string() const override;
  UsePtr dest_;
  std::vector<UsePtr> src_list_;
};

struct SiiIRReturn : public SiiIRCode {
  SiiIRReturn(ValuePtr value)
      : SiiIRCode(SiiIRCodeKind::RETURN), result_(NewUse(this, value)) {
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

  std::string to_string() const override;
  UsePtr result_;
};

} // namespace SiiIR
