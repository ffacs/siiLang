#pragma once
#include <cstdint>
#include <string>

#include "IR/address.h"
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
  ASSIGN = 9,
  GOTO = 10,
  IF_TRUE_GOTO = 11,
  IF_FALSE_GOTO = 12,
  NOPE = 13,
  FUNCTION_DEFINITION = 14,
  ALLOCA = 15
};

struct SiiIRCode;
struct SiiIRBinaryOperation;
struct SiiIRUnaryOperation;
struct SiiIRAssign;
struct SiiIRGoto;
struct SiiIRIfTrueGoto;
struct SiiIRIfFalseGoto;
struct SiiIRNope;
struct SiiIRFunctionDefinition;
struct SiiIRAlloca;
using SiiIRCodePtr = std::shared_ptr<SiiIRCode>;
using SiiIRBinaryOperationPtr = std::shared_ptr<SiiIRBinaryOperation>;
using SiiIRUnaryOperationPtr = std::shared_ptr<SiiIRUnaryOperation>;
using SiiIRAssignPtr = std::shared_ptr<SiiIRAssign>;
using SiiIRGotoPtr = std::shared_ptr<SiiIRGoto>;
using SiiIRIfTrueGotoPtr = std::shared_ptr<SiiIRIfTrueGoto>;
using SiiIRIfFalseGotoPtr = std::shared_ptr<SiiIRIfFalseGoto>;
using SiiIRNopePtr = std::shared_ptr<SiiIRNope>;
using SiiIRFunctionDefinitionPtr = std::shared_ptr<SiiIRFunctionDefinition>;
using SiiIRAllocaPtr = std::shared_ptr<SiiIRAlloca>;

struct SiiIRCode {
  SiiIRCodeKind kind_;
  std::vector<LabelPtr> labels_;
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

struct SiiIRAssign : public SiiIRCode {
  SiiIRAssign(AddressPtr src, AddressPtr dest)
      : SiiIRCode(SiiIRCodeKind::ASSIGN), src_(std::move(src)),
        dest_(std::move(dest)) {}
  std::string to_string() const override;
  AddressPtr src_;
  AddressPtr dest_;
};

struct SiiIRGoto : public SiiIRCode {
  explicit SiiIRGoto(LabelPtr dest)
      : SiiIRCode(SiiIRCodeKind::GOTO), dest_(std::move(dest)) {}
  std::string to_string() const override;
  LabelPtr dest_;
};

struct SiiIRIfTrueGoto : public SiiIRCode {
  SiiIRIfTrueGoto(AddressPtr condition, LabelPtr dest)
      : SiiIRCode(SiiIRCodeKind::IF_TRUE_GOTO),
        condition_(std::move(condition)), dest_(std::move(dest)) {}
  std::string to_string() const override;
  AddressPtr condition_;
  LabelPtr dest_;
};

struct SiiIRIfFalseGoto : public SiiIRCode {
  SiiIRIfFalseGoto(AddressPtr condition, LabelPtr dest)
      : SiiIRCode(SiiIRCodeKind::IF_FALSE_GOTO),
        condition_(std::move(condition)), dest_(std::move(dest)) {}
  std::string to_string() const override;
  AddressPtr condition_;
  LabelPtr dest_;
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
  SiiIRAlloca(AddressPtr dest, uint32_t size)
      : SiiIRCode(SiiIRCodeKind::ALLOCA), dest_(std::move(dest)), size_(size) {}
  std::string to_string() const override;
  AddressPtr dest_;
  uint32_t size_;
};