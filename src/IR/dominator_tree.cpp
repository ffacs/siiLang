#include "IR/dominator_tree.h"
#include <map>

namespace SiiIR {

class UnionFind {
public:
  UnionFind(std::vector<int64_t> &value, size_t size)
      : value_(value), min_ancestor_(size), father_(size) {
    for (size_t i = 0; i < size; ++i) {
      father_[i] = i;
      min_ancestor_[i] = i;
    }
  }

  int64_t get_min_ancestor(int x) {
    merge_path(x);
    return min_ancestor_[x];
  }

  void union_two_nodes(int64_t x, int64_t y) { father_[x] = y; }

private:
  int merge_path(int64_t x) {
    if (father_[x] == x)
      return x;
    int grand = merge_path(father_[x]);
    if (value_[min_ancestor_[father_[x]]] < value_[min_ancestor_[x]]) {
      min_ancestor_[x] = min_ancestor_[father_[x]];
    }
    return father_[x] = grand;
  }

  std::vector<int64_t> &value_;
  std::vector<int64_t> min_ancestor_;
  std::vector<int64_t> father_;
};

class DominatorTreeBuilder {
public:
  DominatorTreeBuilder(FunctionPtr func) : func_(std::move(func)) {}
  DominatorTreePtr build_dominator_tree();
  void assign_index(BasicGroup *basic_group_node, int64_t &index);
  void build_immediate_dominators();
  DominatorTreePtr consturct_dominator_tree();

private:
  FunctionPtr func_;
  std::map<BasicGroup *, int64_t> basic_group_node_to_index_;
  int64_t node_count = 0;
  std::vector<BasicGroup *> index_to_basic_group_node_;
  std::vector<int64_t> father_;
  std::vector<std::vector<int64_t>> previous_ids_;
  // bucket[i] is the list of nodes whose semi-dominator is i.
  std::vector<std::vector<int64_t>> bucket_;
  // semi-dominator[i] is the semi-dominator of node i.
  std::vector<int64_t> semi_dominator_;
  // idom[i] is the immediate dominator of node i.
  std::vector<int64_t> immediate_dominator_;
};

DominatorTreePtr DominatorTreeBuilder::build_dominator_tree() {
  node_count = 0;
  father_.clear();
  father_.resize(func_->basic_groups_.size());
  previous_ids_.clear();
  previous_ids_.resize(func_->basic_groups_.size());
  bucket_.clear();
  bucket_.resize(func_->basic_groups_.size());
  semi_dominator_.clear();
  semi_dominator_.resize(func_->basic_groups_.size());
  immediate_dominator_.clear();
  immediate_dominator_.resize(func_->basic_groups_.size());
  build_immediate_dominators();
  return consturct_dominator_tree();
}

void DominatorTreeBuilder::build_immediate_dominators() {
  assign_index(func_->entry_, node_count);
  UnionFind union_find(semi_dominator_, node_count);
  for (int64_t i = node_count - 1; i > 0; --i) {
    for (auto previous : previous_ids_[i]) {
      semi_dominator_[i] =
          std::min(semi_dominator_[i],
                   semi_dominator_[union_find.get_min_ancestor(previous)]);
    }
    bucket_[semi_dominator_[i]].push_back(i);
    union_find.union_two_nodes(i, father_[i]);
    for (auto node : bucket_[father_[i]]) {
      int64_t min_ancestor = union_find.get_min_ancestor(node);
      if (semi_dominator_[min_ancestor] == semi_dominator_[node]) {
        immediate_dominator_[node] = father_[i];
      } else {
        immediate_dominator_[node] = min_ancestor;
      }
    }
    bucket_[father_[i]].clear();
  }
  for (int64_t i = 1; i < node_count; ++i) {
    if (immediate_dominator_[i] != semi_dominator_[i]) {
      immediate_dominator_[i] = immediate_dominator_[immediate_dominator_[i]];
    }
  }
}

DominatorTreePtr DominatorTreeBuilder::consturct_dominator_tree() {
  DominatorTreePtr dominator_tree = std::make_shared<DominatorTree>(
      std::make_shared<DominatorTreeNode>(func_->entry_));
  for (int64_t i = 1; i < node_count; ++i) {
    DominatorTreeNodePtr new_dominator_tree_node =
        std::make_shared<DominatorTreeNode>(index_to_basic_group_node_[i]);
    const DominatorTreeNodePtr &parent_node =
        dominator_tree->nodes_[immediate_dominator_[i]];
    new_dominator_tree_node->parent_ = parent_node.get();
    parent_node->children_.push_back(new_dominator_tree_node.get());
    dominator_tree->nodes_.push_back(std::move(new_dominator_tree_node));
  }
  return dominator_tree;
}

void DominatorTreeBuilder::assign_index(BasicGroup *basic_group_node,
                                        int64_t &index) {
  int64_t currnt_index = index++;
  basic_group_node_to_index_[basic_group_node] = currnt_index;
  index_to_basic_group_node_.push_back(basic_group_node);
  for (auto *follow : basic_group_node->follows_) {
    int64_t follow_index = 0;
    if (basic_group_node_to_index_.find(follow) ==
        basic_group_node_to_index_.end()) {
      follow_index = index;
      assign_index(follow, index);
      father_[follow_index] = currnt_index;
      semi_dominator_[follow_index] = follow_index;
    } else {
      follow_index = basic_group_node_to_index_[follow];
    }
    previous_ids_[follow_index].push_back(currnt_index);
  }
}

DominatorTreePtr BuildDominatorTree(FunctionPtr func) {
  DominatorTreeBuilder builder(func);
  return builder.build_dominator_tree();
}

} // namespace SiiIR
