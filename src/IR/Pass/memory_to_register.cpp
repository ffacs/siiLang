#include "IR/Pass/memory_to_register.h"
#include "IR/IDF_builder.h"
#include "IR/dominator_tree.h"
#include <map>
#include <set>
#include <stack>
#include <stdexcept>

namespace SiiIR {

static bool CanVariableToRegister(const SiiIR::Value& address) {
  for(const auto& use: address.users_) {
    if(use.user_->kind_ == SiiIRCodeKind::STORE) {
      SiiIRStore* store = static_cast<SiiIRStore*>(use.user_);
      if(store->src_->value_.get() == &address) {
        return false;
      }
    }
  }
  return true;
}

// Insert phi for variable
static void
VariableMemoryToRegister(FunctionPtr&                func,
                         ValuePtr                    variable_address,
                         IDFBuilder*                 idf_builder,
                         std::map<Value*, ValuePtr>& original_variable_map) {
  std::vector<BasicGroup*> def_groups;
  for(const auto& use: variable_address->users_) {
    if(use.user_->kind_ == SiiIRCodeKind::STORE) {
      SiiIRStore* store = static_cast<SiiIRStore*>(use.user_);
      if(store->dest_->value_ == variable_address) {
        def_groups.push_back(use.user_->group_);
      }
    }
  }

  const std::set<BasicGroup*>& bg_to_insert_phis
      = idf_builder->get_IDF(def_groups);
  for(auto& bg: bg_to_insert_phis) {
    auto phi
        = std::make_shared<SiiIRPhi>(variable_address, bg->precedes_.size());
    original_variable_map[phi.get()] = variable_address;
    bg->codes_.push_front(phi);
  }
}

static void ReplaceTemporary(UsePtr*                     use,
                             std::map<Value*, ValuePtr>& temporary_rename_map) {
  const ValuePtr& value_ptr = (*use)->value_;
  if(temporary_rename_map.find(value_ptr.get()) != temporary_rename_map.end()) {
    UsePtr old_use = *use;
    old_use->remove_from_parent();
    *use = NewUse(old_use->user_, temporary_rename_map[value_ptr.get()]);
  }
  return;
}

// Rename variable to temporary
static void
RenamePass(DominatorTreeNode*                      current_node,
           std::map<Value*, std::stack<ValuePtr>>& variable_rename_map,
           std::map<Value*, ValuePtr>&             temporary_rename_map,
           std::map<Value*, ValuePtr>&             original_variable_map,
           FunctionContext&                        ctx) {
  std::map<Value*, size_t> rename_count;
  auto&                    code_list = current_node->basic_group_->codes_;
  for(auto iter = code_list.begin(); iter != code_list.end(); ++iter) {
    auto& code = *iter;
    switch(code.kind_) {
    case SiiIRCodeKind::PHI: {
      SiiIRPhi& phi = static_cast<SiiIRPhi&>(code);
      if(original_variable_map.find(&phi) == original_variable_map.end()) {
        for(auto& src: phi.src_list_) {
          ReplaceTemporary(&src, temporary_rename_map);
        }
        continue;
      }
      ValuePtr variable = original_variable_map[&phi];
      variable_rename_map[variable.get()].push(iter.shared());
      rename_count[variable.get()]++;
      continue;
    }
    case SiiIRCodeKind::LOAD: {
      SiiIRLoad& load   = static_cast<SiiIRLoad&>(code);
      Value*     source = load.src_->value_.get();
      if(variable_rename_map.find(source) != variable_rename_map.end()) {
        temporary_rename_map[&load] = variable_rename_map[source].top();
        code_list.erase(iter);
      }
      continue;
    }
    case SiiIRCodeKind::STORE: {
      SiiIRStore& store         = static_cast<SiiIRStore&>(code);
      Value*      dest_variable = store.dest_->value_.get();
      if(variable_rename_map.find(dest_variable) == variable_rename_map.end()) {
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
      SiiIRBinaryOperation& binary_operation
          = static_cast<SiiIRBinaryOperation&>(code);
      ReplaceTemporary(&binary_operation.lhs_, temporary_rename_map);
      ReplaceTemporary(&binary_operation.rhs_, temporary_rename_map);
      continue;
    }
    case SiiIRCodeKind::NEG: {
      SiiIRUnaryOperation& unary_operation
          = static_cast<SiiIRUnaryOperation&>(code);
      ReplaceTemporary(&unary_operation.operand_, temporary_rename_map);
      continue;
    }
    case SiiIRCodeKind::CONDITION_BRANCH: {
      SiiIRConditionBranch& condition_branch
          = static_cast<SiiIRConditionBranch&>(code);
      ReplaceTemporary(&condition_branch.condition_, temporary_rename_map);
      continue;
    }
    case SiiIRCodeKind::GOTO:
    case SiiIRCodeKind::NOPE: {
      continue;
    }
    case SiiIRCodeKind::ALLOCA: {
      SiiIRAlloca& alloca = static_cast<SiiIRAlloca&>(code);
      if(variable_rename_map.find(&alloca) != variable_rename_map.end()) {
        code_list.erase(iter);
      }
      continue;
    }
    case SiiIRCodeKind::RETURN: {
      SiiIRReturn& ret = static_cast<SiiIRReturn&>(code);
      ReplaceTemporary(&ret.result_, temporary_rename_map);
      continue;
    }
    default: {
      throw std::runtime_error("Unsupported code kind");
    }
    }
  }
  BasicGroup* current_basic_group = current_node->basic_group_;
  for(size_t i = 0; i < current_basic_group->follows_.size(); i++) {
    BasicGroup* follow = current_basic_group->follows_[i];
    auto        iter   = follow->codes_.begin();
    for(size_t j = 0; j < follow->codes_.size(); j++, ++iter) {
      if(iter->kind_ != SiiIRCodeKind::PHI) {
        break;
      }
      SiiIRPhi& phi      = static_cast<SiiIRPhi&>(*iter);
      Value*    variable = original_variable_map[&phi].get();
      if(variable_rename_map.find(variable) != variable_rename_map.end()) {
        for(size_t k = 0; k < follow->precedes_.size(); k++) {
          if(follow->precedes_[k] == current_basic_group) {
            phi.replace_src(k, variable_rename_map[variable].top());
          }
        }
      }
    }
  }
  for(auto& child_node: current_node->children_) {
    RenamePass(child_node,
               variable_rename_map,
               temporary_rename_map,
               original_variable_map,
               ctx);
  }
  for(auto [variable, count]: rename_count) {
    for(size_t i = 0; i < count; i++) {
      variable_rename_map[variable].pop();
    }
  }
}

static bool TryRemoveAllocIfStoreOnly(SiiIRAlloca& alloca) {
  for(auto& use: alloca.users_) {
    if(use.user_->kind_ != SiiIRCodeKind::STORE) {
      return false;
    }
  }
  alloca.remove_from_parent();
  return true;
}

static bool FuncMemoryToRegister(FunctionPtr& func) {
  std::unique_ptr<IDFBuilder>            idf_builder = CreateIDFBuilder(func);
  std::map<Value*, std::stack<ValuePtr>> variable_rename_map;
  std::map<Value*, ValuePtr>             original_variable_map;
  for(auto& code: func->entry_->codes_) {
    if(code.kind_ != SiiIRCodeKind::ALLOCA) {
      continue;
    }
    SiiIRAlloca& alloca_code = static_cast<SiiIRAlloca&>(code);
    if(!CanVariableToRegister(alloca_code)) {
      continue;
    }
    if(TryRemoveAllocIfStoreOnly(alloca_code)) {
      continue;
    }
    VariableMemoryToRegister(func,
                             code.get_iterator().shared(),
                             idf_builder.get(),
                             original_variable_map);
    variable_rename_map[&alloca_code].push(
        Value::undef(Type::GetAimType(alloca_code.type_)));
  }
  if(variable_rename_map.empty()) {
    return false;
  }

  std::map<Value*, ValuePtr> temporary_rename_map;
  RenamePass(idf_builder->get_dom()->root_,
             variable_rename_map,
             temporary_rename_map,
             original_variable_map,
             *func->ctx_);
  return true;
}

void MemoryToRegisterPass::run(FunctionPtr& func) {
  do { } while(FuncMemoryToRegister(func)); }

}  // namespace SiiIR
