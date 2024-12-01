#include "symble_table.h"

SymbolPtr Symbol::symbol(TypePtr type, const std::string &name) {
  return std::make_shared<Symbol>(std::move(type), name);
}

SymbolPtr SymbolTable::find(const std::string &identifier) const {
  auto iter = name_to_symbol_.find(identifier);
  if (iter != name_to_symbol_.end()) {
    return iter->second;
  }
  return nullptr;
}

bool SymbolTable::push(SymbolPtr symbol) {
  if (find(symbol->name_) != nullptr) { return false; }
  name_to_symbol_[symbol->name_] = symbol;
  return true;
}

SymbolTablePtr CreateSymbolTable() {
  return std::make_shared<SymbolTable>();
}
