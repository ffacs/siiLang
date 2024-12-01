#include "context_manager.h"

static ContextPtr CreateContext(Context* father) {
  return std::make_shared<Context>(father);
}

class ContextManagerImpl : public ContextManager {
public:
  ContextManagerImpl();
  Context* current_context() override;
  void push_context() override;
  void pop_context() override;
protected:
  ContextPtr root_;
  Context* current_;
};

ContextManagerImpl::ContextManagerImpl() {
  root_ = CreateContext(nullptr);
  current_ = root_.get();
}

Context* ContextManagerImpl::current_context() {
  return current_;
}

void ContextManagerImpl::push_context() {
  auto new_context = CreateContext(current_);
  current_->children_.push_back(new_context);
  current_ = new_context.get();
}

void ContextManagerImpl::pop_context() {
  current_ = current_->father_;
}

ContextManagerPtr CreateContextManager() {
  return std::make_shared<ContextManagerImpl>();
}
