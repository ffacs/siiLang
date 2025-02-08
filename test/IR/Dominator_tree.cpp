#include "IR/dominator_tree.h"
#include <gtest/gtest.h>
#include <random>

namespace SiiIR{

static FunctionPtr BuildFunction(size_t node_count, size_t extra_edge_count) {
  FunctionPtr func = std::make_shared<Function>();
  func->basic_groups_.push_back(std::make_shared<BasicGroup>());
  func->entry_ = func->basic_groups_[0].get();

  for (size_t i = 1; i < node_count; i++) {
    func->basic_groups_.push_back(std::make_shared<BasicGroup>());
  }
  std::mt19937 mt(std::random_device{}());
  std::uniform_int_distribution<size_t> normal_dist(0, node_count - 1);
  for (int i = 1; i < node_count; i++) {
    BasicGroup* node = func->basic_groups_[i].get();
    size_t father = normal_dist(mt);
    BasicGroup* father_node = func->basic_groups_[father].get();
    father_node->follows_.push_back(node);
    node->precedes_.push_back(father_node);
  }
  
  for (size_t i = 0; i < extra_edge_count; i++) {
    size_t father = normal_dist(mt);
    size_t son = normal_dist(mt);
    BasicGroup* father_node = func->basic_groups_[father].get();
    BasicGroup* son_node = func->basic_groups_[son].get();
    father_node->follows_.push_back(son_node);
    son_node->precedes_.push_back(father_node);
  }
  return func;
}

static void TraverseWithout(BasicGroup* current,
                            BasicGroup* without,
                     std::set<BasicGroup*>& visited) {
  if (current == without) { return; }
  visited.insert(current);
  for (auto son : current->follows_) {
    if (visited.find(son) == visited.end()) {
      TraverseWithout(son, without, visited);
    }
  }
}

static std::set<BasicGroup*> DeleteAndTest(FunctionPtr func, BasicGroup* node) {
  std::set<BasicGroup*> visited;
  std::set<BasicGroup*> result;
  TraverseWithout(func->entry_, node, visited);  
  for (const auto& basic_group : func->basic_groups_) {
    if (visited.find(basic_group.get()) == visited.end()) {
      result.insert(basic_group.get());
    }
  }
  return result;
}

static std::map<BasicGroup*, std::set<BasicGroup*>> GetDominators(FunctionPtr func) {
  std::map<BasicGroup*, std::set<BasicGroup*>> dominators;
  for (const auto& basic_group : func->basic_groups_) {
    auto dominateds = DeleteAndTest(func, basic_group.get());
    for (const auto& dominated : dominateds) {
      dominators[dominated].insert(basic_group.get());
    }
  }
  return dominators;
}
static bool VerifyDominatorTree(DominatorTreeNode* node,
                                FunctionPtr func,
                                std::set<BasicGroup*>& dominators,
                                std::map<BasicGroup*, std::set<BasicGroup*>>& expected) {
  auto current = node->basic_group_node_;
  dominators.insert(current);
  if (expected.find(current) == expected.end() || expected[current] != dominators) {
    return false;    
  }
  for (const auto& child : node->children_) {
    if (!VerifyDominatorTree(child, func, dominators, expected)) {
      return false;
    }
  }
  dominators.erase(current);
  return true;
}

TEST(DominatorTreeTest, BuildDominatorTree) {
  for (size_t i = 0; i < 100; i++) {
    FunctionPtr func = BuildFunction(10, 5);
    DominatorTreePtr tree = BuildDominatorTree(func);
    std::set<BasicGroup*> dominators;
    std::map<BasicGroup*, std::set<BasicGroup*>> expected = GetDominators(func);
    ASSERT_TRUE(VerifyDominatorTree(tree->root_, func, dominators, expected));
  }
  for (size_t i = 0; i < 10; i++) {
    FunctionPtr func = BuildFunction(100, 30);
    DominatorTreePtr tree = BuildDominatorTree(func);
    std::set<BasicGroup*> dominators;
    std::map<BasicGroup*, std::set<BasicGroup*>> expected = GetDominators(func);
    ASSERT_TRUE(VerifyDominatorTree(tree->root_, func, dominators, expected));
  }
  for (size_t i = 0; i < 1; i++) {
    FunctionPtr func = BuildFunction(1000, 300);
    DominatorTreePtr tree = BuildDominatorTree(func);
    std::set<BasicGroup*> dominators;
    std::map<BasicGroup*, std::set<BasicGroup*>> expected = GetDominators(func);
    ASSERT_TRUE(VerifyDominatorTree(tree->root_, func, dominators, expected));
  }
}
}