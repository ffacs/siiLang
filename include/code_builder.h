#pragma once
#include "three_address_code.h"

class CodeBuilder {
 public: 
  virtual ~CodeBuilder() {}
  virtual AddressPtr append_multiply(AddressPtr left, AddressPtr right) = 0;
  virtual AddressPtr append_divide(AddressPtr left, AddressPtr right) = 0;
  virtual AddressPtr append_add(AddressPtr left, AddressPtr right) = 0;
  virtual AddressPtr append_sub(AddressPtr left, AddressPtr right) = 0;
  virtual AddressPtr append_neg(AddressPtr child) = 0;
  virtual AddressPtr append_equal(AddressPtr left, AddressPtr right) = 0;
  virtual AddressPtr append_not_equal(AddressPtr left, AddressPtr right) = 0;
  virtual AddressPtr append_less_than(AddressPtr left, AddressPtr right) = 0;
  virtual AddressPtr append_less_equal(AddressPtr left, AddressPtr right) = 0;
  virtual std::vector<ThreeAddressCodePtr> finish() = 0;
};

std::unique_ptr<CodeBuilder> CreateCodeBuilder();
