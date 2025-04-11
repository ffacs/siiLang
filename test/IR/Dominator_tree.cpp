#include "IR/dominator_tree.h"
#include "IR_test_utils.h"
#include <gtest/gtest.h>

namespace SiiIR {

  static bool VerifyDominatorTree(
      DominatorTreeNode*                                            node,
      FunctionPtr                                                   func,
      std::set< const BasicGroup* >&                                dominators,
      std::map< const BasicGroup*, std::set< const BasicGroup* > >& expected ) {
    auto current = node->basic_group_;
    dominators.insert( current );
    if ( expected.find( current ) == expected.end()
         || expected[ current ] != dominators ) {
      return false;
    }
    for ( const auto& child : node->children_ ) {
      if ( !VerifyDominatorTree( child, func, dominators, expected ) ) {
        return false;
      }
    }
    dominators.erase( current );
    return true;
  }

  TEST( DominatorTreeTest, BuildDominatorTree ) {
    for ( size_t node_count = 1; node_count < 10; ++node_count ) {
      for ( size_t extra_edge_count = 0;
            extra_edge_count < node_count * node_count;
            ++extra_edge_count ) {
        FunctionPtr      func = BuildFunction( node_count, extra_edge_count );
        DominatorTreePtr tree = BuildDominatorTree( func );
        std::set< const BasicGroup* >                                dominators;
        std::map< const BasicGroup*, std::set< const BasicGroup* > > expected
            = GetDominators( func );
        ASSERT_TRUE(
            VerifyDominatorTree( tree->root_, func, dominators, expected ) );
      }
    }
    for ( size_t i = 0; i < 10; i++ ) {
      FunctionPtr                   func = BuildFunction( 100, 30 );
      DominatorTreePtr              tree = BuildDominatorTree( func );
      std::set< const BasicGroup* > dominators;
      std::map< const BasicGroup*, std::set< const BasicGroup* > > expected
          = GetDominators( func );
      ASSERT_TRUE(
          VerifyDominatorTree( tree->root_, func, dominators, expected ) );
    }
    for ( size_t i = 0; i < 1; i++ ) {
      FunctionPtr                   func = BuildFunction( 1000, 300 );
      DominatorTreePtr              tree = BuildDominatorTree( func );
      std::set< const BasicGroup* > dominators;
      std::map< const BasicGroup*, std::set< const BasicGroup* > > expected
          = GetDominators( func );
      ASSERT_TRUE(
          VerifyDominatorTree( tree->root_, func, dominators, expected ) );
    }
  }
}  // namespace SiiIR
