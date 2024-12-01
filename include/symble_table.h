#pragma once
#include "type.h"

#include <string>
#include <memory>
#include <map>

class Symbol;
class SymbolTable;
typedef std::shared_ptr<Symbol> SymbolPtr;
typedef std::shared_ptr<SymbolTable> SymbolTablePtr;

struct Symbol {
  TypePtr type_;
  std::string name_;
  Symbol(TypePtr type, const std::string& name) : type_(std::move(type)), name_(name) {}
  static SymbolPtr symbol(TypePtr type, const std::string& name);
};

struct SymbolTable {
  std::map<std::string, SymbolPtr> name_to_symbol_;

  SymbolPtr find(const std::string& identifier) const;
  bool push(SymbolPtr symbol);
};

SymbolTablePtr CreateSymbolTable();
