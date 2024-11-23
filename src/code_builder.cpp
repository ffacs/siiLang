#include "code_builder.h"

class CodeBuilderImpl : public CodeBuilder {
 public:
  AddressPtr append_multiply(AddressPtr left, AddressPtr right) override;
  AddressPtr append_divide(AddressPtr left, AddressPtr right) override;
  AddressPtr append_add(AddressPtr left, AddressPtr right) override;
  AddressPtr append_sub(AddressPtr left, AddressPtr right) override;
  AddressPtr append_neg(AddressPtr left) override;
  AddressPtr append_equal(AddressPtr left, AddressPtr right) override;
  AddressPtr append_not_equal(AddressPtr left, AddressPtr right) override;
  AddressPtr append_less_than(AddressPtr left, AddressPtr right) override;
  AddressPtr append_less_equal(AddressPtr left, AddressPtr right) override;
  AddressPtr append_assign(AddressPtr left, AddressPtr right) override;
  std::vector<ThreeAddressCodePtr> finish() override;
 protected:
  std::vector<ThreeAddressCodePtr> code_list_;
  uint32_t temproray_variable_count_ = 0;
};

AddressPtr CodeBuilderImpl::append_multiply(AddressPtr left, AddressPtr right) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::MUL);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  auto result = Address::temporary(new_tac.get(), std::to_string(temproray_variable_count_++));
  new_tac->result_ = result;
  code_list_.emplace_back(std::move(new_tac));
  return result;
}

AddressPtr CodeBuilderImpl::append_divide(AddressPtr left, AddressPtr right) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::DIV);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  auto result = Address::temporary(new_tac.get(), std::to_string(temproray_variable_count_++));
  new_tac->result_ = result;
  code_list_.emplace_back(std::move(new_tac));
  return result;
}

AddressPtr CodeBuilderImpl::append_add(AddressPtr left, AddressPtr right) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::ADD);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  auto result = Address::temporary(new_tac.get(), std::to_string(temproray_variable_count_++));
  new_tac->result_ = result;
  code_list_.emplace_back(std::move(new_tac));
  return result;
}

AddressPtr CodeBuilderImpl::append_sub(AddressPtr left, AddressPtr right) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::SUB);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  auto result = Address::temporary(new_tac.get(), std::to_string(temproray_variable_count_++));
  new_tac->result_ = result;
  code_list_.emplace_back(std::move(new_tac));
  return result;
}

AddressPtr CodeBuilderImpl::append_neg(AddressPtr child) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::NEG);
  new_tac->argL_ = std::move(child);
  auto result = Address::temporary(new_tac.get(), std::to_string(temproray_variable_count_++));
  new_tac->result_ = result;
  code_list_.emplace_back(std::move(new_tac));
  return result;
}

AddressPtr CodeBuilderImpl::append_equal(AddressPtr left, AddressPtr right) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::EQUAL);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  auto result = Address::temporary(new_tac.get(), std::to_string(temproray_variable_count_++));
  new_tac->result_ = result;
  code_list_.emplace_back(std::move(new_tac));
  return result;
}

AddressPtr CodeBuilderImpl::append_not_equal(AddressPtr left, AddressPtr right) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::NOT_EQUAL);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  auto result = Address::temporary(new_tac.get(), std::to_string(temproray_variable_count_++));
  new_tac->result_ = result;
  code_list_.emplace_back(std::move(new_tac));
  return result;
}

AddressPtr CodeBuilderImpl::append_less_than(AddressPtr left, AddressPtr right) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::LESS_THAN);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  auto result = Address::temporary(new_tac.get(), std::to_string(temproray_variable_count_++));
  new_tac->result_ = result;
  code_list_.emplace_back(std::move(new_tac));
  return result;
}

AddressPtr CodeBuilderImpl::append_less_equal(AddressPtr left, AddressPtr right) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::LESS_EQUAL);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  auto result = Address::temporary(new_tac.get(), std::to_string(temproray_variable_count_++));
  new_tac->result_ = result;
  code_list_.emplace_back(std::move(new_tac));
  return result;
}

AddressPtr CodeBuilderImpl::append_assign(AddressPtr result, AddressPtr right) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::ASSIGN);
  new_tac->result_ = result;
  new_tac->argL_ = std::move(right);
  code_list_.emplace_back(std::move(new_tac));
  return result;
}

std::vector<ThreeAddressCodePtr> CodeBuilderImpl::finish() {
  return std::move(code_list_);
}

std::unique_ptr<CodeBuilder> CreateCodeBuilder() {
  return std::make_unique<CodeBuilderImpl>();
}
