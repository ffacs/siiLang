#include "IR/Pass/memory_to_register.h"
#include "IR/IDF_builder.h"
#include "IR/dominator_tree.h"
#include <map>
#include <set>
#include <stack>

namespace SiiIR {

static bool CanVariableToRegister(const VariableValuePtr &variable) {
  for (const auto &use : variable->users_) {
    if (use.user_->kind_ == SiiIRCodeKind::STORE) {
      SiiIRStore *store = static_cast<SiiIRStore *>(use.user_);
      if (store->src_->value_ == variable) {
        return false;
      }
    }
  }
  return true;
}

// Insert phi for variable
static bool VariableMemoryToRegister(FunctionPtr &func,
                                     VariableValuePtr variable,
                                     IDFBuilder *idf_builder) {
  std::vector<BasicGroup *> def_groups;
  for (const auto &use : variable->users_) {
    if (use.user_->kind_ == SiiIRCodeKind::STORE) {
      SiiIRStore *store = static_cast<SiiIRStore *>(use.user_);
      if (store->dest_->value_ == variable) {
        def_groups.push_back(use.user_->group_);
      }
    }
  }

  const std::set<BasicGroup *> &bg_to_insert_phis =
      idf_builder->get_IDF(def_groups);
  for (auto &bg : bg_to_insert_phis) {
    auto phi = std::make_shared<SiiIRPhi>(variable, bg->precedes_.size());
    bg->codes_.push_front(phi);
  }
  return true;
}

static void
ReplaceTemporary(UsePtr *use,
                 std::map<TemporaryValue *, ValuePtr> &temporary_rename_map) {
  const ValuePtr &value_ptr = (*use)->value_;
  if (value_ptr->kind_ == ValueKind::TEMPORARY) {
    TemporaryValue *temporary = static_cast<TemporaryValue *>(value_ptr.get());
    if (temporary_rename_map.find(temporary) != temporary_rename_map.end()) {
      UsePtr old_use = *use;
      old_use->remove_from_parent();
      *use = NewUse(old_use->user_, temporary_rename_map[temporary]);
    }
  }
  return;
}

// Rename variable to temporary
static void
RenamePass(DominatorTreeNode *current_node,
           std::map<VariableValue *, std::stack<ValuePtr>> &variable_rename_map,
           std::map<TemporaryValue *, ValuePtr> &temporary_rename_map,
           std::map<TemporaryValue *, VariableValue *> &original_variable_map,
           FunctionContext &ctx) {
  std::map<VariableValue *, size_t> rename_count;
  auto &code_list = current_node->basic_group_->codes_;
  for (auto iter = code_list.begin(); iter != code_list.end(); ++iter) {
    auto &code = *iter;
    switch (code.kind_) {
    case SiiIRCodeKind::PHI: {
      SiiIRPhi &phi = static_cast<SiiIRPhi &>(code);
      if (phi.dest_->value_->kind_ != ValueKind::VARIABLE) {
        for (auto &src : phi.src_list_) {
          ReplaceTemporary(&src, temporary_rename_map);
        }
        continue;
      }
      VariableValue *variable =
          static_cast<VariableValue *>(phi.dest_->value_.get());
      auto new_temporary = ctx.allocate_temporary_value(variable->type_);
      variable_rename_map[variable].push(new_temporary);
      rename_count[variable]++;
      phi.use_setter<0>()(new_temporary);
      original_variable_map[new_temporary.get()] = variable;
      continue;
    }
    case SiiIRCodeKind::LOAD: {
      SiiIRLoad &load = static_cast<SiiIRLoad &>(code);
      VariableValue *source =
          static_cast<VariableValue *>(load.src_->value_.get());
      TemporaryValue *dest =
          static_cast<TemporaryValue *>(load.dest_->value_.get());
      if (variable_rename_map.find(source) != variable_rename_map.end()) {
        temporary_rename_map[dest] = variable_rename_map[source].top();
        code_list.erase(iter);
      }
      continue;
    }
    case SiiIRCodeKind::STORE: {
      SiiIRStore &store = static_cast<SiiIRStore &>(code);
      VariableValue *dest_variable =
          static_cast<VariableValue *>(store.dest_->value_.get());
      if (variable_rename_map.find(dest_variable) ==
          variable_rename_map.end()) {
        ReplaceTemporary(&store.src_, temporary_rename_map);
        continue;
      }
      ReplaceTemporary(&store.src_, temporary_rename_map);
      variable_rename_map[dest_variable].push(store.src_->value_);
      rename_count[dest_variable]++;
      code_list.erase(iter);
      continue;
    }
    case SiiIRCodeKind::ADD:
    case SiiIRCodeKind::SUB:
    case SiiIRCodeKind::MUL:
    case SiiIRCodeKind::DIV:
    case SiiIRCodeKind::EQUAL:
    case SiiIRCodeKind::NOT_EQUAL:
    case SiiIRCodeKind::LESS_THAN:
    case SiiIRCodeKind::LESS_EQUAL: {
      SiiIRBinaryOperation &binary_operation =
          static_cast<SiiIRBinaryOperation &>(code);
      ReplaceTemporary(&binary_operation.lhs_, temporary_rename_map);
      ReplaceTemporary(&binary_operation.rhs_, temporary_rename_map);
      continue;
    }
    case SiiIRCodeKind::NEG: {
      SiiIRUnaryOperation &unary_operation =
          static_cast<SiiIRUnaryOperation &>(code);
      ReplaceTemporary(&unary_operation.operand_, temporary_rename_map);
      continue;
    }
    case SiiIRCodeKind::CONDITION_BRANCH: {
      SiiIRConditionBranch &condition_branch =
          static_cast<SiiIRConditionBranch &>(code);
      ReplaceTemporary(&condition_branch.condition_, temporary_rename_map);
      continue;
    }
    case SiiIRCodeKind::GOTO:
    case SiiIRCodeKind::NOPE: {
      continue;
    }
    case SiiIRCodeKind::ALLOCA: {
      SiiIRAlloca &alloca = static_cast<SiiIRAlloca &>(code);
      if (variable_rename_map.find(static_cast<VariableValue *>(
              alloca.dest_->value_.get())) != variable_rename_map.end()) {
        code_list.erase(iter);
      }
      continue;
    }
    default: {
      throw std::runtime_error("Unsupported code kind");
    }
    }
  }
  BasicGroup *current_basic_group = current_node->basic_group_;
  for (size_t i = 0; i < current_basic_group->follows_.size(); i++) {
    BasicGroup *follow = current_basic_group->follows_[i];
    auto iter = follow->codes_.begin();
    for (size_t j = 0; j < follow->codes_.size(); j++, ++iter) {
      if (iter->kind_ != SiiIRCodeKind::PHI) {
        break;
      }
      SiiIRPhi &phi = static_cast<SiiIRPhi &>(*iter);
      const ValuePtr dest = phi.dest_->value_;
      VariableValue *variable = nullptr;
      if (dest->kind_ == ValueKind::VARIABLE) {
        variable = static_cast<VariableValue *>(dest.get());
      } else {
        variable =
            original_variable_map[static_cast<TemporaryValue *>(dest.get())];
      }
      if (variable_rename_map.find(variable) != variable_rename_map.end()) {
        for (size_t k = 0; k < follow->precedes_.size(); k++) {
          if (follow->precedes_[k] == current_basic_group) {
            phi.replace_src(k, variable_rename_map[variable].top());
          }
        }
      }
    }
  }
  for (auto &child_node : current_node->children_) {
    RenamePass(child_node, variable_rename_map, temporary_rename_map,
               original_variable_map, ctx);
  }
  for (auto [variable, count] : rename_count) {
    for (size_t i = 0; i < count; i++) {
      variable_rename_map[variable].pop();
    }
  }
}

static bool TryRemoveAllocIfStoreOnly(SiiIRAlloca &alloca) {
  const ValuePtr &dest = alloca.dest_->value_;
  for (auto &use : alloca.dest_->value_->users_) {
    if (use.user_->kind_ != SiiIRCodeKind::STORE) {
      return false;
    }
  }
  alloca.remove_from_parent();
  return true;
}

static bool FuncMemoryToRegister(FunctionPtr &func) {
  std::unique_ptr<IDFBuilder> idf_builder = CreateIDFBuilder(func);
  std::map<VariableValue *, std::stack<ValuePtr>> variable_rename_map;
  for (auto &code : func->entry_->codes_) {
    if (code.kind_ != SiiIRCodeKind::ALLOCA) {
      continue;
    }
    SiiIRAlloca &alloca_code = static_cast<SiiIRAlloca &>(code);
    const VariableValuePtr &variable =
        std::static_pointer_cast<VariableValue>(alloca_code.dest_->value_);
    if (!CanVariableToRegister(variable)) {
      continue;
    }
    if (TryRemoveAllocIfStoreOnly(alloca_code)) {
      continue;
    }
    if (VariableMemoryToRegister(func, variable, idf_builder.get())) {
      variable_rename_map[variable.get()].push(Value::undef(variable->type_));
    }
  }
  if (variable_rename_map.empty()) {
    return false;
  }

  std::map<TemporaryValue *, ValuePtr> temporary_rename_map;
  std::map<TemporaryValue *, VariableValue *> original_variable_map;
  RenamePass(idf_builder->get_dom()->root_, variable_rename_map,
             temporary_rename_map, original_variable_map, *func->ctx_);
  return true;
}

void MemoryToRegisterPass::run(FunctionPtr &func) {
  while (FuncMemoryToRegister(func))
    ;
}

} // namespace SiiIR