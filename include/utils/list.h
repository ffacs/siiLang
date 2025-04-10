#pragma once

#include <memory>

namespace SiiIR {
template <typename ValueType, typename ParentType, bool IsConst>
class ListIterator;
template <typename ValueType> class List;

template <typename ValueType>
class ListNode : public std::enable_shared_from_this<ListNode<ValueType>> {
private:
  using ParentType = List<ValueType>;
  friend class List<ValueType>;
  friend class ListIterator<ValueType, ParentType, true>;
  friend class ListIterator<ValueType, ParentType, false>;
  ParentType *parent_;
  std::shared_ptr<ListNode> next_;
  std::shared_ptr<ListNode> prev_;

public:
  ListNode(ParentType *parent)
      : parent_(parent), next_(nullptr), prev_(nullptr) {}
  ListNode() : parent_(nullptr), next_(nullptr), prev_(nullptr) {}

  virtual ~ListNode() = default;

  ListIterator<ValueType, ParentType, false> get_iterator() {
    return ListIterator<ValueType, ParentType, false>(this);
  }

  ListIterator<ValueType, ParentType, true> get_iterator() const {
    return ListIterator<ValueType, ParentType, true>(this);
  }

  ParentType *get_parent() { return parent_; }
  void set_parent(ParentType *parent) { parent_ = parent; }

  void remove_from_parent() {
    if (parent_) {
      parent_->erase(get_iterator());
    }
  }
};

template <typename ValueType, typename ParentType, bool IsConst>
class ListIterator {
private:
  ListNode<ValueType> *node_;
  ListNode<ValueType> *next_;
  ListNode<ValueType> *prev_;

  void update_node(ListNode<ValueType> *new_node) {
    node_ = new_node;
    next_ = node_->next_.get();
    prev_ = node_->prev_.get();
  }

  friend struct ListNode<ValueType>;
  friend class List<ValueType>;
  ListIterator(ListNode<ValueType> *node)
      : node_(node), next_(node->next_.get()), prev_(node->prev_.get()) {}

public:
  typename std::conditional<IsConst, std::shared_ptr<const ValueType>,
                            std::shared_ptr<ValueType>>::type
  shared() const {
    return std::dynamic_pointer_cast<ValueType>(node_->shared_from_this());
  }

  ListIterator<ValueType, ParentType, IsConst> &operator++() {
    update_node(next_);
    return *this;
  }

  ListIterator<ValueType, ParentType, IsConst> &operator--() {
    update_node(prev_);
    return *this;
  }

  typename std::conditional<IsConst, const ValueType *, ValueType *>::type
  operator->() const {
    return dynamic_cast<ValueType *>(node_);
  }

  typename std::conditional<IsConst, const ValueType &, ValueType &>::type
  operator*() const {
    return *static_cast<ValueType *>(node_);
  }

  bool
  operator==(const ListIterator<ValueType, ParentType, true> &other) const {
    return node_ == other.node_;
  }

  bool
  operator==(const ListIterator<ValueType, ParentType, false> &other) const {
    return node_ == other.node_;
  }

  bool
  operator!=(const ListIterator<ValueType, ParentType, true> &other) const {
    return node_ != other.node_;
  }

  bool
  operator!=(const ListIterator<ValueType, ParentType, false> &other) const {
    return node_ != other.node_;
  }
};

template <typename ValueType> class List {
public:
  using ConstIterType = ListIterator<ValueType, List<ValueType>, true>;
  using IterType = ListIterator<ValueType, List<ValueType>, false>;
  using NodeType = ListNode<ValueType>;

  List() : size_(0) { construct_dummy_head_and_tail(); }

  List &operator=(List &&other) {
    break_down();
    dummy_head_ = std::move(other.dummy_head_);
    dummy_tail_ = std::move(other.dummy_tail_);
    size_ = other.size_;
    other.size_ = 0;
    other.construct_dummy_head_and_tail();
    return *this;
  }

  ~List() { break_down(); }

  IterType begin() { return IterType(dummy_head_->next_.get()); }

  ConstIterType begin() const {
    return ConstIterType(dummy_head_->next_.get());
  }

  const ValueType &operator[](size_t index) const {
    auto iter = begin();
    for (size_t i = 0; i < index; ++i) {
      ++iter;
    }
    return *iter;
  }

  ValueType &operator[](size_t index) {
    auto iter = begin();
    for (size_t i = 0; i < index; ++i) {
      ++iter;
    }
    return *iter;
  }

  IterType end() { return IterType(dummy_tail_.get()); }

  ConstIterType end() const { return ConstIterType(dummy_tail_.get()); }

  void push_back(std::shared_ptr<NodeType> node) {
    insert_before(end(), std::move(node));
  }

  void push_front(std::shared_ptr<NodeType> node) {
    insert_before(begin(), std::move(node));
  }

  void insert_after(const IterType &iter, std::shared_ptr<NodeType> node) {
    size_++;
    NodeType &iter_node = *iter.node_;
    node->next_ = iter_node.next_;
    node->prev_ = iter_node.next_->prev_;
    iter_node.next_->prev_ = node;
    iter_node.next_ = node;
    node->set_parent(this);
  }

  void insert_before(const IterType &iter, std::shared_ptr<NodeType> node) {
    size_++;
    NodeType &iter_node = *iter.node_;
    node->next_ = iter_node.prev_->next_;
    node->prev_ = iter_node.prev_;
    iter_node.prev_->next_ = node;
    iter_node.prev_ = node;
    node->set_parent(this);
  }

  std::shared_ptr<ListNode<ValueType>> erase(const IterType &iter) {
    size_--;
    NodeType &iter_node = *iter.node_;
    std::shared_ptr<ListNode<ValueType>> result = iter_node.next_->prev_;
    iter_node.prev_->next_ = iter_node.next_;
    iter_node.next_->prev_ = iter_node.prev_;
    iter_node.next_.reset();
    iter_node.prev_.reset();
    iter_node.set_parent(nullptr);
    return result;
  }

  size_t size() const { return size_; }

private:
  void break_down() {
    NodeType *current = dummy_head_.get();
    while (current) {
      NodeType *next = current->next_.get();
      current->next_.reset();
      current->prev_.reset();
      current = next;
    }
  }

  void construct_dummy_head_and_tail() {
    dummy_head_ = std::make_shared<NodeType>(this);
    dummy_tail_ = std::make_shared<NodeType>(this);
    dummy_head_->next_ = dummy_tail_;
    dummy_tail_->prev_ = dummy_head_;
  }

  std::shared_ptr<NodeType> dummy_head_;
  std::shared_ptr<NodeType> dummy_tail_;
  size_t size_;
};

} // namespace SiiIR