#include "IR/IDF_builder.h"
#include "IR_test_utils.h"
#include <gtest/gtest.h>

namespace SiiIR {

  static bool VerifyDomninaceFrotier(
      const BasicGroup*              bg,
      const std::set< BasicGroup* >& DFs,
      const std::map< const BasicGroup*, std::set< const BasicGroup* > >&
          dominators ) {
    for ( const auto& df : DFs ) {
      const std::set< const BasicGroup* >& dominators_of_df
          = dominators.at( df );
      if ( dominators_of_df.find( bg ) != dominators_of_df.end() ) {
        throw std::runtime_error( "DF is dominated by bg" );
        return false;
      }
      bool has_predecessor_be_dominated = false;
      for ( const BasicGroup* pred : df->precedes_ ) {
        const auto& dominators_of_pred = dominators.at( pred );
        if ( dominators_of_pred.find( bg ) != dominators_of_pred.end() ) {
          has_predecessor_be_dominated = true;
          break;
        }
      }
      if ( !has_predecessor_be_dominated ) {
        throw std::runtime_error( "No predecessor is dominated by bg" );
      }
    }
    return true;
  }

  TEST( IDFBuilder, DominanceFrontier ) {
    for ( size_t node_count = 1; node_count < 10; ++node_count ) {
      for ( size_t extra_edge_count = 0;
            extra_edge_count < node_count * node_count;
            ++extra_edge_count ) {
        FunctionPtr func        = BuildFunction( node_count, extra_edge_count );
        auto        dominators  = GetDominators( func );
        auto        IDF_builder = CreateIDFBuilder( func );
        for ( const auto& bg : func->basic_groups_ ) {
          auto DFs = IDF_builder->get_DF( bg.get() );
          ASSERT_TRUE( VerifyDomninaceFrotier( bg.get(), DFs, dominators ) );
        }
      }
    }
    for ( size_t i = 0; i < 10; ++i ) {
      size_t      node_count  = 100 + i;
      FunctionPtr func        = BuildFunction( node_count, i / 2 );
      auto        dominators  = GetDominators( func );
      auto        IDF_builder = CreateIDFBuilder( func );
      for ( const auto& bg : func->basic_groups_ ) {
        auto DFs = IDF_builder->get_DF( bg.get() );
        ASSERT_TRUE( VerifyDomninaceFrotier( bg.get(), DFs, dominators ) );
      }
    }
    for ( size_t i = 0; i < 1; ++i ) {
      size_t      node_count  = 1000 + i;
      FunctionPtr func        = BuildFunction( node_count, node_count / 2 );
      auto        dominators  = GetDominators( func );
      auto        IDF_builder = CreateIDFBuilder( func );
      for ( const auto& bg : func->basic_groups_ ) {
        auto DFs = IDF_builder->get_DF( bg.get() );
        ASSERT_TRUE( VerifyDomninaceFrotier( bg.get(), DFs, dominators ) );
      }
    }
  }

  static std::vector< std::vector< BasicGroup* > >
  GetAllSubsets( const std::vector< BasicGroupPtr >& bgs ) {
    std::vector< std::vector< BasicGroup* > > subsets;
    size_t                                    n = bgs.size();
    for ( size_t i = 0; i < ( 1 << n ); ++i ) {
      std::vector< BasicGroup* > subset;
      for ( size_t j = 0; j < n; ++j ) {
        if ( i & ( 1 << j ) ) {
          subset.push_back( bgs[ j ].get() );
        }
      }
      subsets.push_back( subset );
    }
    return subsets;
  }

  static std::set< BasicGroup* >
  GetIDF( FunctionPtr func, const std::vector< BasicGroup* >& bgs ) {
    std::set< BasicGroup* >    result       = {};
    std::vector< BasicGroup* > working_list = bgs;
    auto                       IDF_builder  = CreateIDFBuilder( func );
    while ( true ) {
      bool changed = false;
      for ( const BasicGroup* bg : working_list ) {
        std::set< BasicGroup* > DFs = IDF_builder->get_DF( bg );
        for ( BasicGroup* df : DFs ) {
          if ( result.insert( df ).second ) {
            changed = true;
          }
        }
      }
      if ( !changed ) {
        break;
      }
      working_list = { result.begin(), result.end() };
    }
    return result;
  }

  TEST( IDF_builder, IteratedDominanceFrontier ) {
    for ( size_t node_count = 1; node_count < 10; ++node_count ) {
      for ( size_t extra_edge_count = 0;
            extra_edge_count < node_count * node_count;
            ++extra_edge_count ) {
        FunctionPtr func        = BuildFunction( node_count, extra_edge_count );
        auto        dominators  = GetDominators( func );
        auto        IDF_builder = CreateIDFBuilder( func );
        const auto& all_subsets = GetAllSubsets( func->basic_groups_ );
        for ( const auto& subset : all_subsets ) {
          auto IDF = IDF_builder->get_IDF( subset );
          ASSERT_EQ( GetIDF( func, subset ), IDF );
        }
      }
    }
  }

}  // namespace SiiIR
