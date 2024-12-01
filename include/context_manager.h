#pragma once
#include <vector>
#include <memory>

#include "symble_table.h"

struct Context;
struct ContextManager;
typedef std::shared_ptr<Context> ContextPtr;
typedef std::shared_ptr<ContextManager> ContextManagerPtr;

struct Context {
  Context(Context* father) : father_(father) {
    symble_table_ = std::make_shared<SymbolTable>();
  }
  Context* father_;
  std::vector<ContextPtr> children_;
  SymbolTablePtr symble_table_;
};

class ContextManager {
public:
  virtual ~ContextManager() {}
  virtual Context* current_context() = 0;
  virtual void push_context() = 0;
  virtual void pop_context() = 0;
};

ContextManagerPtr CreateContextManager();
