#pragma once
#include "IR/function_ctx.h"
#include "IR/value.h"
#include "front/type.h"
#include <map>
#include <memory>
#include <vector>

namespace front {
class Symbol;
class FunctionSymbol;
class VariableSymbol;
class SymbolTable;
struct SymbolContext;
struct ContextManager;
using SymbolPtr = std::shared_ptr<Symbol>;
using FunctionSymbolPtr = std::shared_ptr<FunctionSymbol>;
using VariableSymbolPtr = std::shared_ptr<VariableSymbol>;
using SymbolTablePtr = std::shared_ptr<SymbolTable>;
using SymbolContextPtr = std::shared_ptr<SymbolContext>;
using ContextManagerPtr = std::shared_ptr<ContextManager>;

enum class SymbolKind : uint8_t { VARIABLE, FUNCTION };

class Symbol {
public:
  TypePtr type_;
  SymbolKind kind_;
  Symbol(SymbolKind kind, TypePtr type) : kind_(kind), type_(std::move(type)) {}

  static FunctionSymbolPtr NewFunctionSymbol(TypePtr type,
                                             SiiIR::FunctionValuePtr func);
  static VariableSymbolPtr NewVariableSymbol(TypePtr type,
                                             SiiIR::ValuePtr address);
};

class VariableSymbol : public Symbol {
public:
  SiiIR::ValuePtr address_;
  SiiIR::TypePtr ir_type_;

  VariableSymbol(TypePtr type, SiiIR::ValuePtr address)
      : Symbol(SymbolKind::VARIABLE, std::move(type)),
        address_(std::move(address)) {}
};

class FunctionSymbol : public Symbol {
public:
  SiiIR::FunctionValuePtr func_;

  FunctionSymbol(TypePtr type, SiiIR::FunctionValuePtr func)
      : Symbol(SymbolKind::FUNCTION, std::move(type)), func_(std::move(func)) {}
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
  virtual void enter_function(SiiIR::TypePtr type) = 0;
  virtual SiiIR::FunctionContextPtr leave_function() = 0;
};

ContextManagerPtr CreateContextManager();

} // namespace front