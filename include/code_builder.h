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
  virtual AddressPtr append_assign(AddressPtr left, AddressPtr right) = 0;
  virtual LabelPtr   new_label(const std::string& name = "") = 0;
  virtual void       append_if_else(AddressPtr expression, LabelPtr true_label, LabelPtr false_label) = 0;
  virtual void       append_goto(LabelPtr target_label) = 0;
  virtual void       append_label(LabelPtr label) = 0;
  virtual void       append_nope() = 0;
  virtual std::vector<ThreeAddressCodePtr> finish() = 0;
};

std::unique_ptr<CodeBuilder> CreateCodeBuilder();
