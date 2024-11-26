#pragma once
#include "AST.h"

enum class TACOperator : uint32_t {
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
  NOPE = 13
};

enum class AddressType : uint32_t {
  VARIABLE = 0,
  CONSTANT = 1,
  TEMPORARY = 2
};

struct Address;
struct ThreeAddressCode;
struct Label;
typedef std::shared_ptr<Address> AddressPtr;
typedef std::shared_ptr<ThreeAddressCode> ThreeAddressCodePtr;
typedef std::shared_ptr<Label> LabelPtr;

struct Address {
  Address(AddressType type) : type_(type) {}
  AddressType type_;
  
  virtual std::string to_string() const = 0;

  static std::shared_ptr<Address> variable(const std::string& name);
  static std::shared_ptr<Address> constant(const std::string& literal);
  static std::shared_ptr<Address> temporary(ThreeAddressCode* src, const std::string& name);
};

struct VariableAddress : public Address {
  VariableAddress(const std::string& name) : Address(AddressType::VARIABLE), name_(name) {}
  std::string to_string() const override;
  std::string name_;
};

struct ConstantAddress : public Address {
  ConstantAddress(const std::string& literal) : Address(AddressType::CONSTANT), literal_(literal) {}
  std::string to_string() const override;
  std::string literal_;
};

struct TemporaryAddress : public Address {
  TemporaryAddress(ThreeAddressCode* src, const std::string& name) : Address(AddressType::TEMPORARY), name_(name) {}
  std::string to_string() const override;
  ThreeAddressCode* src_;
  std::string name_;
};

struct Label {
  ThreeAddressCode* dest_;
  std::string name_;
  Label(ThreeAddressCode* dest, const std::string& name) : dest_(dest), name_(name) {}
  std::string to_string() const;
};

struct ThreeAddressCode {
  TACOperator operator_;  
  AddressPtr argL_;
  AddressPtr argR_;
  AddressPtr result_;
  std::vector<LabelPtr> lable_list_;
  LabelPtr jump_dest_;

  ThreeAddressCode(TACOperator ope) : operator_(ope) {}

  std::string to_string() const;
};
