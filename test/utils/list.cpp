#include "utils/list.h"
#include <gtest/gtest.h>

namespace SiiIR {

class IntNode : public ListNode< IntNode > {
public:
  int  value_;
  int& counter_;
  IntNode(int value, int& counter)
      : value_(value)
      , counter_(counter) {
    ++counter;
  }
  ~IntNode() { --counter_; }
};

TEST(List, push_front) {
  int counter = 0;
  {

    List< IntNode > list;
    for ( int i = 0; i < 10; i++ ) {
      list.push_front(std::make_shared< IntNode >(i, counter));
    }
    auto iter = list.begin();
    for ( int i = 0; i < 10; i++, ++iter ) {
      EXPECT_EQ(iter->value_, 9 - i);
    }
    EXPECT_EQ(10, counter);
  }
  EXPECT_EQ(0, counter);
}

TEST(List, push_back) {
  int counter = 0;
  {
    List< IntNode > list;
    for ( int i = 0; i < 10; i++ ) {
      list.push_back(std::make_shared< IntNode >(i, counter));
    }
    auto iter = list.begin();
    for ( int i = 0; i < 10; i++, ++iter ) {
      EXPECT_EQ(iter->value_, i);
    }
    EXPECT_EQ(10, counter);
  }
  EXPECT_EQ(0, counter);
}

TEST(List, erase) {
  int counter = 0;
  {
    List< IntNode > list, b;
    for ( int i = 0; i < 10; i++ ) {
      list.push_back(std::make_shared< IntNode >(i, counter));
    }
    auto iter = list.begin();
    for ( int i = 0; i < 10; i++, ++iter ) {
      EXPECT_EQ(iter->value_, i);
    }
    iter = list.begin();
    for ( int i = 0; i < 10; i++, ++iter ) {
      if ( i % 2 == 0 ) {
        auto     node     = list.erase(iter);
        IntNode& int_node = static_cast< IntNode& >(*node);
        EXPECT_EQ(int_node.value_, i);
        b.push_back(node);
      }
    }
    EXPECT_EQ(iter, list.end());

    EXPECT_EQ(list.size(), 5);
    iter = list.begin();
    for ( int i = 1; i < 10; i += 2, ++iter ) {
      EXPECT_EQ(iter->value_, i);
    }
    EXPECT_EQ(iter, list.end());

    EXPECT_EQ(b.size(), 5);
    auto b_iter = b.begin();
    for ( int i = 0; i < 10; i += 2, ++b_iter ) {
      EXPECT_EQ(b_iter->value_, i);
    }
    EXPECT_EQ(counter, 10);
  }
  EXPECT_EQ(counter, 0);
}

TEST(List, move_constructor) {
  int counter = 0;
  {
    List< IntNode > b;
    {
      List< IntNode > a;
      for ( int i = 0; i < 10; i++ ) {
        a.push_back(std::make_shared< IntNode >(i, counter));
      }

      b = std::move(a);
      EXPECT_EQ(a.size(), 0);
      EXPECT_EQ(b.size(), 10);
      auto b_iter = b.begin();
      for ( int i = 0; i < 10; i++, ++b_iter ) {
        IntNode& int_node = static_cast< IntNode& >(*b_iter);
        EXPECT_EQ(int_node.value_, i);
      }
      EXPECT_EQ(a.begin(), a.end());
    }
    auto b_iter = b.begin();
    for ( int i = 0; i < 10; i++, ++b_iter ) {
      IntNode& int_node = static_cast< IntNode& >(*b_iter);
      EXPECT_EQ(int_node.value_, i);
    }
    EXPECT_EQ(counter, 10);
  }
  EXPECT_EQ(counter, 0);
}

TEST(List, erase_from_parent) {
  int counter = 0;
  {
    List< IntNode >                           a;
    std::vector< std::shared_ptr< IntNode > > nodes;
    for ( int i = 0; i < 10; i++ ) {
      nodes.push_back(std::make_shared< IntNode >(i, counter));
      a.push_back(nodes.back());
    }
    EXPECT_EQ(a.size(), 10);
    for ( int i = 0; i < 10; i++ ) {
      nodes[ i ]->remove_from_parent();
      EXPECT_EQ(a.size(), 10 - i - 1);
      auto iter = a.begin();
      for ( int j = i + 1; j < 10; j++, ++iter ) {
        EXPECT_EQ(iter->value_, j);
      }
    }
    EXPECT_EQ(a.size(), 0);
  }
  EXPECT_EQ(0, counter);
}

}  // namespace SiiIR
