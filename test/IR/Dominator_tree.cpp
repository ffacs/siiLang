#include "IR/dominator_tree.h"
#include <gtest/gtest.h>
#include <random>

namespace SiiIR{

static CFGPtr BuildCFG(size_t node_count, size_t extra_edge_count) {
  CFGPtr cfg = std::make_shared<CFG>();
  cfg->basic_groups_.push_back(std::make_shared<BasicGroupNode>());
  cfg->entry_ = cfg->basic_groups_[0].get();

  for (size_t i = 1; i < node_count; i++) {
    cfg->basic_groups_.push_back(std::make_shared<BasicGroupNode>());
  }
  std::mt19937 mt(std::random_device{}());
  std::uniform_int_distribution<size_t> normal_dist(0, node_count - 1);
  for (int i = 1; i < node_count; i++) {
    BasicGroupNode* node = cfg->basic_groups_[i].get();
    size_t father = normal_dist(mt);
    BasicGroupNode* father_node = cfg->basic_groups_[father].get();
    father_node->follows_.push_back(node);
    node->precedes_.push_back(father_node);
  }
  
  for (size_t i = 0; i < extra_edge_count; i++) {
    size_t father = normal_dist(mt);
    size_t son = normal_dist(mt);
    BasicGroupNode* father_node = cfg->basic_groups_[father].get();
    BasicGroupNode* son_node = cfg->basic_groups_[son].get();
    father_node->follows_.push_back(son_node);
    son_node->precedes_.push_back(father_node);
  }
  return cfg;
}

static void TraverseWithout(BasicGroupNode* current,
                            BasicGroupNode* without,
                     std::set<BasicGroupNode*>& visited) {
  if (current == without) { return; }
  visited.insert(current);
  for (auto son : current->follows_) {
    if (visited.find(son) == visited.end()) {
      TraverseWithout(son, without, visited);
    }
  }
}

static std::set<BasicGroupNode*> DeleteAndTest(CFGPtr cfg, BasicGroupNode* node) {
  std::set<BasicGroupNode*> visited;
  std::set<BasicGroupNode*> result;
  TraverseWithout(cfg->entry_, node, visited);  
  for (const auto& basic_group : cfg->basic_groups_) {
    if (visited.find(basic_group.get()) == visited.end()) {
      result.insert(basic_group.get());
    }
  }
  return result;
}

static std::map<BasicGroupNode*, std::set<BasicGroupNode*>> GetDominators(CFGPtr cfg) {
  std::map<BasicGroupNode*, std::set<BasicGroupNode*>> dominators;
  for (const auto& basic_group : cfg->basic_groups_) {
    auto dominateds = DeleteAndTest(cfg, basic_group.get());
    for (const auto& dominated : dominateds) {
      dominators[dominated].insert(basic_group.get());
    }
  }
  return dominators;
}
static bool VerifyDominatorTree(DominatorTreeNode* node,
                                CFGPtr cfg,
                                std::set<BasicGroupNode*>& dominators,
                                std::map<BasicGroupNode*, std::set<BasicGroupNode*>>& expected) {
  auto current = node->basic_group_node_;
  dominators.insert(current);
  if (expected.find(current) == expected.end() || expected[current] != dominators) {
    return false;    
  }
  for (const auto& child : node->children_) {
    if (!VerifyDominatorTree(child, cfg, dominators, expected)) {
      return false;
    }
  }
  dominators.erase(current);
  return true;
}

TEST(DominatorTreeTest, BuildDominatorTree) {
  for (size_t i = 0; i < 100; i++) {
    CFGPtr cfg = BuildCFG(10, 5);
    DominatorTreePtr tree = BuildDominatorTree(cfg);
    std::set<BasicGroupNode*> dominators;
    std::map<BasicGroupNode*, std::set<BasicGroupNode*>> expected = GetDominators(cfg);
    ASSERT_TRUE(VerifyDominatorTree(tree->root_, cfg, dominators, expected));
  }
  for (size_t i = 0; i < 10; i++) {
    CFGPtr cfg = BuildCFG(100, 30);
    DominatorTreePtr tree = BuildDominatorTree(cfg);
    std::set<BasicGroupNode*> dominators;
    std::map<BasicGroupNode*, std::set<BasicGroupNode*>> expected = GetDominators(cfg);
    ASSERT_TRUE(VerifyDominatorTree(tree->root_, cfg, dominators, expected));
  }
  for (size_t i = 0; i < 1; i++) {
    CFGPtr cfg = BuildCFG(1000, 300);
    DominatorTreePtr tree = BuildDominatorTree(cfg);
    std::set<BasicGroupNode*> dominators;
    std::map<BasicGroupNode*, std::set<BasicGroupNode*>> expected = GetDominators(cfg);
    ASSERT_TRUE(VerifyDominatorTree(tree->root_, cfg, dominators, expected));
  }
}
}