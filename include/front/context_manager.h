#pragma once
#include "IR/address.h"
#include "IR/function_ctx.h"
#include "front/type.h"
#include <map>
#include <memory>
#include <vector>

namespace front {
class Symbol;
class SymbolTable;
struct SymbolContext;
struct ContextManager;
typedef std::shared_ptr<Symbol> SymbolPtr;
typedef std::shared_ptr<SymbolTable> SymbolTablePtr;
typedef std::shared_ptr<SymbolContext> SymbolContextPtr;
typedef std::shared_ptr<ContextManager> ContextManagerPtr;

struct Symbol {
  TypePtr type_;
  SiiIR::AddressPtr address_;
  Symbol(TypePtr type, SiiIR::AddressPtr address)
      : type_(std::move(type)), address_(std::move(address)) {}
  static SymbolPtr symbol(TypePtr type, SiiIR::AddressPtr address);
};

struct SymbolTable {
  std::map<std::string, SymbolPtr> name_to_symbol_;

  SymbolPtr find(const std::string &identifier) const;
  void push(const std::string &identifier, SymbolPtr symbol);
};

SymbolTablePtr CreateSymbolTable();

struct SymbolContext {
  SymbolContext(SymbolContext *father) : father_(father) {
    symble_table_ = std::make_shared<SymbolTable>();
  }
  SymbolContext *father_;
  std::vector<SymbolContextPtr> children_;
  SymbolTablePtr symble_table_;
};

class ContextManager {
public:
  virtual ~ContextManager() {}
  virtual SymbolContext *symbol_ctx() = 0;
  virtual void push_symbol_ctx() = 0;
  virtual void pop_symbol_ctx() = 0;
  virtual void append_variable(const std::string &name, SymbolPtr variable) = 0;
  virtual void append_function(const std::string &name, SymbolPtr function) = 0;

  virtual SiiIR::FunctionContext *function_ctx() = 0;
  virtual void enter_function() = 0;
  virtual SiiIR::FunctionContextPtr leave_function() = 0;
};

ContextManagerPtr CreateContextManager();

} // namespace front