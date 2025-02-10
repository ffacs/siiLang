#pragma once
#include <cstdint>
#include <string>

#include "IR/address.h"

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

struct SiiIRCode {
  SiiIRCodeKind kind_;
  LabelPtr label_;
  explicit SiiIRCode(SiiIRCodeKind kind) : kind_(kind) {}

  virtual std::string to_string() const;
  virtual ~SiiIRCode() = default;
};

struct SiiIRBinaryOperation : public SiiIRCode {
  SiiIRBinaryOperation(SiiIRCodeKind kind, AddressPtr lhs, AddressPtr rhs,
                       AddressPtr result)
      : SiiIRCode(kind), lhs_(std::move(lhs)), rhs_(std::move(rhs)),
        result_(std::move(result)) {}
  std::string to_string() const override;
  AddressPtr lhs_;
  AddressPtr rhs_;
  AddressPtr result_;
};

struct SiiIRUnaryOperation : public SiiIRCode {
  SiiIRUnaryOperation(SiiIRCodeKind kind, AddressPtr operand, AddressPtr result)
      : SiiIRCode(kind), operand_(std::move(operand)),
        result_(std::move(result)) {}
  std::string to_string() const override;
  AddressPtr operand_;
  AddressPtr result_;
};

struct SiiIRGoto : public SiiIRCode {
  explicit SiiIRGoto(LabelPtr dest)
      : SiiIRCode(SiiIRCodeKind::GOTO), dest_label_(std::move(dest)) {}
  std::string to_string() const override;
  LabelPtr dest_label_;
};

struct SiiIRConditionBarnch : public SiiIRCode {
  SiiIRConditionBarnch(AddressPtr condition, LabelPtr true_label, LabelPtr false_label)
      : SiiIRCode(SiiIRCodeKind::CONDITION_BRANCH),
        condition_(std::move(condition)), true_label_(std::move(true_label)),
                                          false_label_(std::move(false_label)) {}
  std::string to_string() const override;
  AddressPtr condition_;
  LabelPtr true_label_;
  LabelPtr false_label_;
};

struct SiiIRNope : public SiiIRCode {
  SiiIRNope() : SiiIRCode(SiiIRCodeKind::NOPE) {}
  std::string to_string() const override;
};

struct SiiIRFunctionDefinition : public SiiIRCode {
  SiiIRFunctionDefinition(FunctionAddressPtr function)
      : SiiIRCode(SiiIRCodeKind::FUNCTION_DEFINITION),
        function_(std::move(function)) {}
  std::string to_string() const override;
  FunctionAddressPtr function_;
};

struct SiiIRAlloca : public SiiIRCode {
  SiiIRAlloca(VariableAddressPtr dest, uint32_t size)
      : SiiIRCode(SiiIRCodeKind::ALLOCA), dest_(std::move(dest)), size_(size) {}
  std::string to_string() const override;
  VariableAddressPtr dest_;
  uint32_t size_;
};

struct SiiIRLoad : public SiiIRCode {
  SiiIRLoad(AddressPtr src, AddressPtr dest)
      : SiiIRCode(SiiIRCodeKind::LOAD), src_(std::move(src)),
        dest_(std::move(dest)) {}
  std::string to_string() const override;
  AddressPtr src_;
  AddressPtr dest_;
};

struct SiiIRStore : public SiiIRCode {
  SiiIRStore(AddressPtr src, AddressPtr dest)
      : SiiIRCode(SiiIRCodeKind::STORE), src_(std::move(src)),
        dest_(std::move(dest)) {}
  std::string to_string() const override;
  AddressPtr src_;
  AddressPtr dest_;
};

} // namespace SiiIR
