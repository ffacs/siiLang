#pragma once
#include <cstdint>
#include <string>

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
  PHI = 17
};

struct BasicGroup;
struct SiiIRCode;
struct SiiIRBinaryOperation;
struct SiiIRUnaryOperation;
struct SiiIRAssign;
struct SiiIRGoto;
struct SiiIRNope;
struct SiiIRConditionBarnch;
struct SiiIRFunctionDefinition;
struct SiiIRAlloca;
struct SiiIRStore;
struct SiiIRLoad;
struct SiiIRPhi;
using SiiIRCodePtr = std::shared_ptr<SiiIRCode>;
using SiiIRBinaryOperationPtr = std::shared_ptr<SiiIRBinaryOperation>;
using SiiIRUnaryOperationPtr = std::shared_ptr<SiiIRUnaryOperation>;
using SiiIRGotoPtr = std::shared_ptr<SiiIRGoto>;
using SiiIRNopePtr = std::shared_ptr<SiiIRNope>;
using SiiIRConditionBarnchPtr = std::shared_ptr<SiiIRConditionBarnch>;
using SiiIRFunctionDefinitionPtr = std::shared_ptr<SiiIRFunctionDefinition>;
using SiiIRAllocaPtr = std::shared_ptr<SiiIRAlloca>;
using SiiIRStorePtr = std::shared_ptr<SiiIRStore>;
using SiiIRLoadPtr = std::shared_ptr<SiiIRLoad>;
using SiiIRPhiPtr = std::shared_ptr<SiiIRPhi>;

struct SiiIRCode : public ListNode<SiiIRCode> {
  SiiIRCodeKind kind_;
  LabelPtr label_;
  BasicGroup* group_ = nullptr;
  explicit SiiIRCode(SiiIRCodeKind kind) : kind_(kind) {}

  virtual std::string to_string() const;
  virtual ~SiiIRCode() = default;
};

struct SiiIRBinaryOperation : public SiiIRCode {
  SiiIRBinaryOperation(SiiIRCodeKind kind, ValuePtr lhs, ValuePtr rhs,
                       ValuePtr result)
      : SiiIRCode(kind), lhs_(std::move(lhs)), rhs_(std::move(rhs)),
        result_(std::move(result)) {}
  std::string to_string() const override;
  ValuePtr lhs_;
  ValuePtr rhs_;
  ValuePtr result_;
};

struct SiiIRUnaryOperation : public SiiIRCode {
  SiiIRUnaryOperation(SiiIRCodeKind kind, ValuePtr operand, ValuePtr result)
      : SiiIRCode(kind), operand_(std::move(operand)),
        result_(std::move(result)) {}
  std::string to_string() const override;
  ValuePtr operand_;
  ValuePtr result_;
};

struct SiiIRGoto : public SiiIRCode {
  explicit SiiIRGoto(LabelPtr dest)
      : SiiIRCode(SiiIRCodeKind::GOTO), dest_label_(std::move(dest)) {}
  std::string to_string() const override;
  LabelPtr dest_label_;
};

struct SiiIRConditionBarnch : public SiiIRCode {
  SiiIRConditionBarnch(TemporaryValuePtr condition, LabelPtr true_label,
                       LabelPtr false_label)
      : SiiIRCode(SiiIRCodeKind::CONDITION_BRANCH),
        condition_(std::move(condition)), true_label_(std::move(true_label)),
        false_label_(std::move(false_label)) {}
  std::string to_string() const override;
  ValuePtr condition_;
  LabelPtr true_label_;
  LabelPtr false_label_;
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
      : SiiIRCode(SiiIRCodeKind::ALLOCA), dest_(std::move(dest)), size_(size) {}
  std::string to_string() const override;
  VariableValuePtr dest_;
  uint32_t size_;
};

struct SiiIRLoad : public SiiIRCode {
  SiiIRLoad(VariableValuePtr src, TemporaryValuePtr dest)
      : SiiIRCode(SiiIRCodeKind::LOAD), src_(std::move(src)),
        dest_(std::move(dest)) {}
  std::string to_string() const override;
  VariableValuePtr src_;
  TemporaryValuePtr dest_;
};

struct SiiIRStore : public SiiIRCode {
  SiiIRStore(ValuePtr src, ValuePtr dest)
      : SiiIRCode(SiiIRCodeKind::STORE), src_(std::move(src)),
        dest_(std::move(dest)) {}
  std::string to_string() const override;
  ValuePtr src_;
  ValuePtr dest_;
};

struct SiiIRPhi : public SiiIRCode {
  SiiIRPhi(VariableValuePtr variable, size_t src_size)
      : SiiIRCode(SiiIRCodeKind::PHI), dest_(variable),
        src_list_(src_size, std::move(variable)),
        from_list_(src_size, nullptr) {}

  std::string to_string() const override;
  ValuePtr dest_;
  std::vector<ValuePtr> src_list_;
  std::vector<LabelPtr> from_list_;
};

} // namespace SiiIR
