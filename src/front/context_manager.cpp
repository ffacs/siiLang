#include "front/context_manager.h"
#include <map>
#include <stdexcept>

SymbolPtr Symbol::symbol(TypePtr type, AddressPtr address) {
  return std::make_shared<Symbol>(std::move(type), std::move(address));
}

SymbolPtr SymbolTable::find(const std::string &identifier) const {
  auto iter = name_to_symbol_.find(identifier);
  if (iter != name_to_symbol_.end()) {
    return iter->second;
  }
  return nullptr;
}

void SymbolTable::push(const std::string& name, SymbolPtr symbol) {
  auto old_symbol = find(name);
  if (old_symbol == nullptr) {
    name_to_symbol_[name] = symbol;
    return;
  }
  bool type_the_same = *old_symbol->type_ != *symbol->type_;
  if (old_symbol->type_->kind_ == TypeKind::FUNCTION) {
    bool has_definition = false;
    const FunctionAddress& old_function_address = static_cast<const FunctionAddress&>(*old_symbol->address_);
    const FunctionAddress& new_function_address = static_cast<const FunctionAddress&>(*symbol->address_);
    if (new_function_address.codes_ != nullptr) {
      if (old_function_address.codes_ == nullptr) {
        name_to_symbol_[name] = symbol;
      } else {
        throw std::invalid_argument("Redefination of " + name);
      }
    }
  } else {
    if (type_the_same) {
      throw std::invalid_argument("Redefination of " + name + "with different type.");
    } else {
      throw std::invalid_argument("Redefination of " + name);
    }
  }
}

SymbolTablePtr CreateSymbolTable() {
  return std::make_shared<SymbolTable>();
}

static SymbolContextPtr CreateSymbolContext(SymbolContext* father) {
  return std::make_shared<SymbolContext>(father);
}

static FunctionContextPtr CreateFunctionContext() {
  return std::make_shared<FunctionContext>();
}

TemporaryAddressPtr FunctionContext::allocate_temporary_address() {
  auto new_temporary = std::make_shared<TemporaryAddress>(nullptr, "tmp"+std::to_string(temporary_addresses_.size()));
  temporary_addresses_.emplace_back(new_temporary);
  return new_temporary;
}

VariableAddressPtr FunctionContext::allocate_variable_address() {
  auto new_variable = std::make_shared<VariableAddress>("var"+std::to_string(variable_addresses_.size()));
  variable_addresses_.emplace_back(new_variable);
  return new_variable;
}

VariableAddressPtr FunctionContext::allocate_parameter_address() {
  auto new_variable = std::make_shared<VariableAddress>("par"+std::to_string(variable_addresses_.size()));
  variable_addresses_.emplace_back(new_variable);
  return new_variable;
}

void  FunctionContext::rename_all_addresses() {
  uint32_t idx = 0;
  for (auto& variable : parameter_addresses_) {
    variable->name_ = std::to_string(idx++);
  }
  for (auto& variable : variable_addresses_) {
    variable->name_ = std::to_string(idx++);
  }
  for (auto& temporary : temporary_addresses_) {
    temporary->name_ = std::to_string(idx++);
  }
}

class ContextManagerImpl : public ContextManager {
public:
  ContextManagerImpl();
  SymbolContext* symbol_ctx() override;
  void push_symbol_ctx() override;
  void pop_symbol_ctx() override;
  FunctionContext* function_ctx() override;

  void append_variable(const std::string& name, SymbolPtr variable) override;
  void append_function(const std::string& name, SymbolPtr function) override;

  void enter_function()  override;
  void leave_function()  override;
protected:
  SymbolContextPtr root_symbol_ctx_;
  SymbolContext* current_symbol_ctx_;
  std::map<std::string, FunctionContextPtr> function_ctxes_;
  std::shared_ptr<FunctionContext> current_function_ctx_;
};

ContextManagerImpl::ContextManagerImpl() {
  root_symbol_ctx_ = CreateSymbolContext(nullptr);
  current_symbol_ctx_ = root_symbol_ctx_.get();
}

SymbolContext* ContextManagerImpl::symbol_ctx() {
  return current_symbol_ctx_;
}

void ContextManagerImpl::push_symbol_ctx() {
  auto new_context = CreateSymbolContext(current_symbol_ctx_);
  current_symbol_ctx_->children_.push_back(new_context);
  current_symbol_ctx_ = new_context.get();
}

void ContextManagerImpl::pop_symbol_ctx() {
  current_symbol_ctx_ = current_symbol_ctx_->father_;
}

FunctionContext* ContextManagerImpl::function_ctx() {
  return current_function_ctx_.get();
}

void ContextManagerImpl::append_variable(const std::string& name, SymbolPtr variable) {
  current_symbol_ctx_->symble_table_->push(name, variable);
}

void ContextManagerImpl::append_function(const std::string &name, SymbolPtr function) {
  root_symbol_ctx_->symble_table_->push(name, function);
}

void ContextManagerImpl::enter_function() {
  current_function_ctx_ = CreateFunctionContext();
}

void ContextManagerImpl::leave_function() {
  current_function_ctx_ = nullptr;
}

ContextManagerPtr CreateContextManager() {
  return std::make_shared<ContextManagerImpl>();
}
