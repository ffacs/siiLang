#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "IR/type.h"

namespace SiiIR {
struct FunctionContext;
typedef std::shared_ptr<FunctionContext> FunctionContextPtr;
enum class AddressKind : uint32_t {
  VARIABLE = 0,
  CONSTANT = 1,
  TEMPORARY = 2,
  FUNCTION = 3,
  LABEL = 4
};

struct SiiIRCode;
struct Address;
struct VariableAddress;
struct ConstantAddress;
struct TemporaryAddress;
struct FunctionAddress;
struct Label;
struct LabelFuture;
using SiiIRCodePtr = std::shared_ptr<SiiIRCode>;
typedef std::shared_ptr<Address> AddressPtr;
typedef std::shared_ptr<VariableAddress> VariableAddressPtr;
typedef std::shared_ptr<ConstantAddress> ConstantAddressPtr;
typedef std::shared_ptr<TemporaryAddress> TemporaryAddressPtr;
typedef std::shared_ptr<FunctionAddress> FunctionAddressPtr;
typedef std::shared_ptr<Label> LabelPtr;
typedef std::shared_ptr<LabelFuture> LabelFuturePtr;

struct Address {
  explicit Address(AddressKind kind, TypePtr type) : kind_(kind), type_(type) {}
  AddressKind kind_;
  TypePtr type_;

  virtual std::string to_string() const = 0;

  static VariableAddressPtr variable(const std::string &name, TypePtr type);
  static ConstantAddressPtr constant(const std::string &literal, TypePtr type);
  static TemporaryAddressPtr temporary(SiiIRCode *src, const std::string &name,
                                       TypePtr type);
  static FunctionAddressPtr
  Function(std::shared_ptr<std::vector<SiiIRCodePtr>> codes,
           FunctionContextPtr ctx, const std::string &name, TypePtr type);
};

struct VariableAddress : public Address {
  explicit VariableAddress(const std::string &name, TypePtr type)
      : Address(AddressKind::VARIABLE, std::move(type)), name_(name) {}
  std::string to_string() const override;
  std::string name_;
};

struct TemporaryAddress : public Address {
  TemporaryAddress(SiiIRCode *src, std::string name, TypePtr type)
      : Address(AddressKind::TEMPORARY, std::move(type)),
        name_(std::move(name)) {}
  std::string to_string() const override;
  SiiIRCode *src_{};
  std::string name_;
};

struct ConstantAddress : public TemporaryAddress {
  explicit ConstantAddress(const std::string &literal, TypePtr type)
      : TemporaryAddress(nullptr, "", std::move(type)), literal_(literal) {
    kind_ = AddressKind::CONSTANT;
  }
  std::string to_string() const override;
  std::string literal_;
};

struct FunctionAddress : public Address {
  FunctionAddress(std::shared_ptr<std::vector<SiiIRCodePtr>> codes,
                  FunctionContextPtr ctx, std::string name, TypePtr type)
      : Address(AddressKind::FUNCTION, std::move(type)),
        codes_(std::move(codes)), ctx_(std::move(ctx)), name_(std::move(name)) {
  }
  std::string to_string() const override;
  std::shared_ptr<std::vector<SiiIRCodePtr>> codes_;
  FunctionContextPtr ctx_;
  std::string name_;
};

struct Label : public Address {
  SiiIRCode *dest_code_;
  std::string name_;
  Label(SiiIRCode *dest, std::string name)
      : Address(AddressKind::LABEL, nullptr), dest_code_(dest),
        name_(std::move(name)) {}
  std::string to_string() const;
  bool operator<(const Label &rhs) const { return name_ < rhs.name_; }
};

struct LabelFuture {
  LabelPtr *dest_;
  LabelFuture(LabelPtr *dest) : dest_(dest) {}
  void set_label(LabelPtr label) { *dest_ = label; }
};

} // namespace SiiIR
