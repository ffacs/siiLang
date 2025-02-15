#include "IR/IDF_builder.h"
#include "IR/dominator_tree.h"
#include <map>
#include <queue>
#include <set>

namespace SiiIR {

struct IDFBuilderImpl : public IDFBuilder {
  std::map<const DominatorTreeNode *, std::set<DominatorTreeNode *>>
      dominance_frontiers_;
  std::map<const BasicGroup *, DominatorTreeNode *>
      bg_to_dominator_tree_node_map_;
  DominatorTreePtr dominator_tree_;

  IDFBuilderImpl(FunctionPtr func) : IDFBuilder(std::move(func)) { initial(); }
  void initial();
  void build_dominance_frontiers_of_node(const DominatorTreeNode *node);
  DominatorTreePtr get_dom() override { return dominator_tree_; }
  std::set<BasicGroup *> get_DF(const BasicGroup *) override;
  std::set<BasicGroup *> get_IDF(const std::vector<BasicGroup *> &) override;
};

static bool IsDominatorOf(const DominatorTreeNode *dom,
                          const DominatorTreeNode *node) {
  while (node->level > dom->level) {
    node = node->parent_;
  }
  return node == dom;
}

std::set<BasicGroup *> IDFBuilderImpl::get_DF(const BasicGroup *group) {
  const DominatorTreeNode *node = bg_to_dominator_tree_node_map_.at(group);
  std::set<BasicGroup *> result;
  for (const DominatorTreeNode *df_node : dominance_frontiers_.at(node)) {
    result.insert(df_node->basic_group_);
  }
  return result;
}

std::set<BasicGroup *>
IDFBuilderImpl::get_IDF(const std::vector<BasicGroup *> &groups) {
  std::queue<DominatorTreeNode *> working_list;
  std::set<DominatorTreeNode *> IDF_set;
  for (const BasicGroup *group : groups) {
    working_list.push(bg_to_dominator_tree_node_map_.at(group));
  }
  while (!working_list.empty()) {
    DominatorTreeNode *node = working_list.front();
    working_list.pop();
    for (DominatorTreeNode *df : dominance_frontiers_[node]) {
      if (IDF_set.insert(df).second) {
        working_list.push(df);
      }
    }
  }
  std::set<BasicGroup *> result;
  for (DominatorTreeNode *node : IDF_set) {
    result.insert(node->basic_group_);
  }
  return result;
}

void IDFBuilderImpl::initial() {
  dominator_tree_ = BuildDominatorTree(func_);
  // Init bg_to_dominator_tree_node_map_
  for (const DominatorTreeNodePtr &node : dominator_tree_->nodes_) {
    bg_to_dominator_tree_node_map_[node->basic_group_] = node.get();
  }
  build_dominance_frontiers_of_node(dominator_tree_->root_);
}

void IDFBuilderImpl::build_dominance_frontiers_of_node(
    const DominatorTreeNode *node) {

  BasicGroup *basic_group = node->basic_group_;
  dominance_frontiers_[node] = {};
  for (BasicGroup *succ : basic_group->follows_) {
    DominatorTreeNode *succ_node = bg_to_dominator_tree_node_map_.at(succ);
    if (!IsDominatorOf(node, succ_node)) {
      dominance_frontiers_[node].insert(succ_node);
    }
  }
  for (DominatorTreeNode *child : node->children_) {
    build_dominance_frontiers_of_node(child);
    for (DominatorTreeNode *child_frontier : dominance_frontiers_.at(child)) {
      if (!IsDominatorOf(node, child_frontier)) {
        dominance_frontiers_[node].insert(child_frontier);
      }
    }
  }
}

std::unique_ptr<IDFBuilder> CreateIDFBuilder(FunctionPtr func) {
  return std::make_unique<IDFBuilderImpl>(std::move(func));
}

} // namespace SiiIR