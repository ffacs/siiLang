#include "front/context_manager.h"
#include <map>
#include <stdexcept>

using namespace SiiIR;

namespace front {
SymbolPtr Symbol::symbol(TypePtr type, ValuePtr value) {
  return std::make_shared<Symbol>(std::move(type), std::move(value));
}

SymbolPtr SymbolTable::find(const std::string &identifier) const {
  auto iter = name_to_symbol_.find(identifier);
  if (iter != name_to_symbol_.end()) {
    return iter->second;
  }
  return nullptr;
}

void SymbolTable::push(const std::string &name, SymbolPtr symbol) {
  auto old_symbol = find(name);
  if (old_symbol == nullptr) {
    name_to_symbol_[name] = symbol;
    return;
  }
  bool type_the_same = *old_symbol->type_ != *symbol->type_;
  if (old_symbol->type_->kind_ == TypeKind::FUNCTION) {
    bool has_definition = false;
    const FunctionValue &old_function_value =
        static_cast<const FunctionValue &>(*old_symbol->value_);
    const FunctionValue &new_function_value =
        static_cast<const FunctionValue &>(*symbol->value_);
    if (new_function_value.codes_ != nullptr) {
      if (old_function_value.codes_ == nullptr) {
        name_to_symbol_[name] = symbol;
      } else {
        throw std::invalid_argument("Redefination of " + name);
      }
    }
  } else {
    if (type_the_same) {
      throw std::invalid_argument("Redefination of " + name +
                                  "with different type.");
    } else {
      throw std::invalid_argument("Redefination of " + name);
    }
  }
}

SymbolTablePtr CreateSymbolTable() { return std::make_shared<SymbolTable>(); }

static SymbolContextPtr CreateSymbolContext(SymbolContext *father) {
  return std::make_shared<SymbolContext>(father);
}

static FunctionContextPtr CreateFunctionContext() {
  return std::make_shared<FunctionContext>();
}
class ContextManagerImpl : public ContextManager {
public:
  ContextManagerImpl();
  SymbolContext *symbol_ctx() override;
  void push_symbol_ctx() override;
  void pop_symbol_ctx() override;
  FunctionContext *function_ctx() override;

  void append_variable(const std::string &name, SymbolPtr variable) override;
  void append_function(const std::string &name, SymbolPtr function) override;

  void enter_function() override;
  FunctionContextPtr leave_function() override;

protected:
  SymbolContextPtr root_symbol_ctx_;
  SymbolContext *current_symbol_ctx_;
  std::map<std::string, FunctionContextPtr> function_ctxes_;
  std::shared_ptr<FunctionContext> current_function_ctx_;
};

ContextManagerImpl::ContextManagerImpl() {
  root_symbol_ctx_ = CreateSymbolContext(nullptr);
  current_symbol_ctx_ = root_symbol_ctx_.get();
}

SymbolContext *ContextManagerImpl::symbol_ctx() { return current_symbol_ctx_; }

void ContextManagerImpl::push_symbol_ctx() {
  auto new_context = CreateSymbolContext(current_symbol_ctx_);
  current_symbol_ctx_->children_.push_back(new_context);
  current_symbol_ctx_ = new_context.get();
}

void ContextManagerImpl::pop_symbol_ctx() {
  current_symbol_ctx_ = current_symbol_ctx_->father_;
}

FunctionContext *ContextManagerImpl::function_ctx() {
  return current_function_ctx_.get();
}

void ContextManagerImpl::append_variable(const std::string &name,
                                         SymbolPtr variable) {
  current_symbol_ctx_->symble_table_->push(name, variable);
}

void ContextManagerImpl::append_function(const std::string &name,
                                         SymbolPtr function) {
  root_symbol_ctx_->symble_table_->push(name, function);
}

void ContextManagerImpl::enter_function() {
  current_function_ctx_ = CreateFunctionContext();
}

FunctionContextPtr ContextManagerImpl::leave_function() {
  auto result = current_function_ctx_;
  current_function_ctx_ = nullptr;
  return result;
}

ContextManagerPtr CreateContextManager() {
  return std::make_shared<ContextManagerImpl>();
}

} // namespace front
