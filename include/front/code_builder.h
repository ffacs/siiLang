#pragma once
#include "IR/address.h"

namespace front {

class CodeBuilder;
typedef std::shared_ptr<CodeBuilder> CodeBuilderPtr;

class CodeBuilder {
public:
  virtual ~CodeBuilder() {}
  virtual void append_multiply(SiiIR::AddressPtr left, SiiIR::AddressPtr right,
                               SiiIR::TemporaryAddressPtr result) = 0;
  virtual void append_divide(SiiIR::AddressPtr left, SiiIR::AddressPtr right,
                             SiiIR::TemporaryAddressPtr result) = 0;
  virtual void append_add(SiiIR::AddressPtr left, SiiIR::AddressPtr right,
                          SiiIR::TemporaryAddressPtr result) = 0;
  virtual void append_sub(SiiIR::AddressPtr left, SiiIR::AddressPtr right,
                          SiiIR::TemporaryAddressPtr result) = 0;
  virtual void append_neg(SiiIR::AddressPtr child, SiiIR::TemporaryAddressPtr result) = 0;
  virtual void append_equal(SiiIR::AddressPtr left, SiiIR::AddressPtr right,
                            SiiIR::TemporaryAddressPtr result) = 0;
  virtual void append_not_equal(SiiIR::AddressPtr left, SiiIR::AddressPtr right,
                                SiiIR::TemporaryAddressPtr result) = 0;
  virtual void append_less_than(SiiIR::AddressPtr left, SiiIR::AddressPtr right,
                                SiiIR::TemporaryAddressPtr result) = 0;
  virtual void append_less_equal(SiiIR::AddressPtr left, SiiIR::AddressPtr right,
                                 SiiIR::TemporaryAddressPtr result) = 0;
  virtual SiiIR::AddressPtr append_assign(SiiIR::AddressPtr dest, SiiIR::AddressPtr src) = 0;
  virtual SiiIR::LabelPtr new_label(const std::string &name = "") = 0;
  virtual void append_if_true_goto(SiiIR::AddressPtr expression,
                                   SiiIR::LabelPtr target_label) = 0;
  virtual void append_if_false_goto(SiiIR::AddressPtr expression,
                                    SiiIR::LabelPtr target_label) = 0;
  virtual void append_goto(SiiIR::LabelPtr target_label) = 0;
  virtual void append_label(SiiIR::LabelPtr label) = 0;
  virtual void append_nope() = 0;
  virtual void append_function(SiiIR::FunctionAddressPtr left) = 0;
  virtual void append_alloca(SiiIR::AddressPtr variable, uint32_t bytes) = 0;
  virtual std::vector<SiiIR::SiiIRCodePtr> finish() = 0;
};

CodeBuilderPtr CreateCodeBuilder();
} // namespace front
