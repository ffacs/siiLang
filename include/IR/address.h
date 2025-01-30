#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace SiiIR {
enum class AddressType : uint32_t {
  VARIABLE = 0,
  CONSTANT = 1,
  TEMPORARY = 2,
  FUNCTION = 3
};

struct SiiIRCode;
struct Address;
struct VariableAddress;
struct ConstantAddress;
struct TemporaryAddress;
struct FunctionAddress;
struct Label;
using SiiIRCodePtr = std::shared_ptr<SiiIRCode>;
typedef std::shared_ptr<Address> AddressPtr;
typedef std::shared_ptr<VariableAddress> VariableAddressPtr;
typedef std::shared_ptr<ConstantAddress> ConstantAddressPtr;
typedef std::shared_ptr<TemporaryAddress> TemporaryAddressPtr;
typedef std::shared_ptr<FunctionAddress> FunctionAddressPtr;
typedef std::shared_ptr<Label> LabelPtr;

struct Address {
  explicit Address(AddressType type) : type_(type) {}
  AddressType type_;

  virtual std::string to_string() const = 0;

  static VariableAddressPtr variable(const std::string &name);
  static ConstantAddressPtr constant(const std::string &literal);
  static TemporaryAddressPtr temporary(SiiIRCode *src, const std::string &name);
  static FunctionAddressPtr
  function(std::shared_ptr<std::vector<SiiIRCodePtr>> codes,
           const std::string &name);
};

struct VariableAddress : public Address {
  explicit VariableAddress(const std::string &name)
      : Address(AddressType::VARIABLE), name_(name) {}
  std::string to_string() const override;
  std::string name_;
};

struct TemporaryAddress : public Address {
  TemporaryAddress(SiiIRCode *src, std::string name)
      : Address(AddressType::TEMPORARY), name_(std::move(name)) {}
  std::string to_string() const override;
  SiiIRCode *src_{};
  std::string name_;
};

struct ConstantAddress : public TemporaryAddress {
  explicit ConstantAddress(const std::string &literal)
      : TemporaryAddress(nullptr, ""), literal_(literal) {
    type_ = AddressType::CONSTANT;
  }
  std::string to_string() const override;
  std::string literal_;
};


struct FunctionAddress : public Address {
  FunctionAddress(std::shared_ptr<std::vector<SiiIRCodePtr>> codes,
                  std::string name)
      : Address(AddressType::FUNCTION), codes_(std::move(codes)),
        name_(std::move(name)) {}
  std::string to_string() const override;
  std::shared_ptr<std::vector<SiiIRCodePtr>> codes_;
  std::string name_;
};

struct Label {
  SiiIRCode *dest_code_;
  std::string name_;
  Label(SiiIRCode *dest, std::string name)
      : dest_code_(dest), name_(std::move(name)) {}
  std::string to_string() const;
  bool operator<(const Label &rhs) const { return name_ < rhs.name_; }
};
} // namespace SiiIR
