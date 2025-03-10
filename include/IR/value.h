#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <functional>

#include "IR/type.h"
#include "utils/list.h"

namespace SiiIR {
struct FunctionContext;
typedef std::shared_ptr<FunctionContext> FunctionContextPtr;
enum class ValueKind : uint32_t {
  VARIABLE = 0,
  CONSTANT = 1,
  TEMPORARY = 2,
  FUNCTION = 3,
  LABEL = 4,
  UNDEF = 5
};

struct Use;
struct SiiIRCode;
struct Value;
struct VariableValue;
struct ConstantValue;
struct TemporaryValue;
struct FunctionValue;
struct UndefValue;
struct Label;
struct LabelFuture;
using SiiIRCodePtr = std::shared_ptr<SiiIRCode>;
typedef std::shared_ptr<Value> ValuePtr;
typedef std::shared_ptr<VariableValue> VariableValuePtr;
typedef std::shared_ptr<ConstantValue> ConstantValuePtr;
typedef std::shared_ptr<TemporaryValue> TemporaryValuePtr;
typedef std::shared_ptr<FunctionValue> FunctionValuePtr;
typedef std::shared_ptr<UndefValue> UndefValuePtr;
typedef std::shared_ptr<Label> LabelPtr;
typedef std::shared_ptr<LabelFuture> LabelFuturePtr;

struct Value {
  explicit Value(ValueKind kind, TypePtr type) : kind_(kind), type_(type) {}
  ValueKind kind_;
  TypePtr type_;
  List<Use> users_;

  virtual std::string to_string() const = 0;

  static VariableValuePtr variable(const std::string &name, TypePtr type,
                                   TypePtr allocated_type);
  static ConstantValuePtr constant(const std::string &literal, TypePtr type);
  static TemporaryValuePtr temporary(SiiIRCode *src, const std::string &name,
                                     TypePtr type);
  static UndefValuePtr undef(TypePtr type);
  static FunctionValuePtr
  Function(std::shared_ptr<std::vector<SiiIRCodePtr>> codes,
           FunctionContextPtr ctx, const std::string &name, TypePtr type);
};

struct VariableValue : public Value {
  explicit VariableValue(const std::string &name, TypePtr type,
                         TypePtr allocated_type)
      : Value(ValueKind::VARIABLE, std::move(type)), name_(name),
        allocated_type_(std::move(allocated_type)) {}

  std::string to_string() const override;
  std::string name_;
  TypePtr allocated_type_;
};

struct TemporaryValue : public Value {
  TemporaryValue(SiiIRCode *src, std::string name, TypePtr type)
      : Value(ValueKind::TEMPORARY, std::move(type)), name_(std::move(name)) {}
  std::string to_string() const override;
  SiiIRCode *src_{};
  std::string name_;
};

struct ConstantValue : public TemporaryValue {
  explicit ConstantValue(const std::string &literal, TypePtr type)
      : TemporaryValue(nullptr, "", std::move(type)), literal_(literal) {
    kind_ = ValueKind::CONSTANT;
  }
  std::string to_string() const override;
  std::string literal_;
};

struct UndefValue : public Value {
  explicit UndefValue(TypePtr type)
      : Value(ValueKind::UNDEF, std::move(type)) {}
  std::string to_string() const override;
};

struct FunctionValue : public Value {
  FunctionValue(std::shared_ptr<std::vector<SiiIRCodePtr>> codes,
                FunctionContextPtr ctx, std::string name, TypePtr type)
      : Value(ValueKind::FUNCTION, std::move(type)), codes_(std::move(codes)),
        ctx_(std::move(ctx)), name_(std::move(name)) {}
  std::string to_string() const override;
  std::shared_ptr<std::vector<SiiIRCodePtr>> codes_;
  FunctionContextPtr ctx_;
  std::string name_;
};

struct Label : public Value {
  SiiIRCode *dest_code_;
  std::string name_;
  Label(SiiIRCode *dest, std::string name)
      : Value(ValueKind::LABEL, nullptr), dest_code_(dest),
        name_(std::move(name)) {}
  std::string to_string() const;
  bool operator<(const Label &rhs) const { return name_ < rhs.name_; }
};

struct LabelFuture {
  std::function<void (LabelPtr)> label_setter_;

  LabelFuture(std::function<void(LabelPtr label)> label_setter) : label_setter_(std::move(label_setter)) {}
  void set_label(LabelPtr label) { label_setter_(label); }
};

} // namespace SiiIR
