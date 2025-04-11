#include "IR/function.h"
#include "IR/function_ctx.h"
#include <set>
#include <sstream>

#include <map>

namespace SiiIR {

  class FunctionBuilder {
  private:
    std::vector< SiiIRCodePtr >       source_codes_;
    FunctionContextPtr                ctx_;
    std::map< SiiIRCode*, size_t >    code_to_index_;
    std::map< Label*, BasicGroupPtr > label_to_node_;
    std::vector< BasicGroupPtr >      basic_groups_;

    BasicGroup* build_basic_group_starting_from( size_t start ) {
      BasicGroupPtr result = nullptr;
      if ( source_codes_[ start ]->label_ != nullptr ) {
        if ( label_to_node_.find( source_codes_[ start ]->label_.get() )
             != label_to_node_.end() ) {
          return label_to_node_[ source_codes_[ start ]->label_.get() ].get();
        }
        result = std::make_shared< BasicGroup >();
        label_to_node_[ source_codes_[ start ]->label_.get() ] = result;
      } else {
        throw std::runtime_error(
            "Building a basic group with a non-label code at the beginning." );
      }
      size_t current_line = start;

      List< SiiIRCode >& target_codes = result->codes_;
      while ( current_line < source_codes_.size() ) {
        auto current = source_codes_[ current_line ];
        if ( current_line != start && current->label_ != nullptr ) {
          target_codes.push_back(
              std::make_shared< SiiIRGoto >( current->label_ ) );
          BasicGroup* next_group
              = build_basic_group_starting_from( current_line );
          result->follows_.push_back( next_group );
          next_group->precedes_.push_back( result.get() );
          break;
        }
        target_codes.push_back( current );
        if ( current->kind_ == SiiIRCodeKind::GOTO ) {
          const SiiIRGoto* goto_code
              = static_cast< const SiiIRGoto* >( current.get() );
          const LabelPtr& dest_label = std::static_pointer_cast< Label >(
              goto_code->dest_label_->value_ );
          BasicGroup* next_group = build_basic_group_starting_from(
              code_to_index_[ dest_label->dest_code_ ] );
          result->follows_.push_back( next_group );
          next_group->precedes_.push_back( result.get() );
          break;
        }
        if ( current->kind_ == SiiIRCodeKind::CONDITION_BRANCH ) {
          const SiiIRConditionBranch* condition_branch
              = static_cast< const SiiIRConditionBranch* >( current.get() );
          const LabelPtr& true_label = std::static_pointer_cast< Label >(
              condition_branch->true_label_->value_ );
          const LabelPtr& false_label = std::static_pointer_cast< Label >(
              condition_branch->false_label_->value_ );
          BasicGroup* true_group = build_basic_group_starting_from(
              code_to_index_[ true_label->dest_code_ ] );
          result->follows_.push_back( true_group );
          true_group->precedes_.push_back( result.get() );

          BasicGroup* false_group = build_basic_group_starting_from(
              code_to_index_[ false_label->dest_code_ ] );
          result->follows_.push_back( false_group );
          false_group->precedes_.push_back( result.get() );
          break;
        }
        current_line++;
      }

      basic_groups_.push_back( result );
      return result.get();
    }

  public:
    FunctionBuilder( std::vector< SiiIRCodePtr > source_codes,
                     FunctionContextPtr          ctx )
        : source_codes_( std::move( source_codes ) )
        , ctx_( std::move( ctx ) ) {}

    FunctionPtr build( std::string name ) {
      FunctionPtr result_func             = std::make_shared< Function >();
      // Create an entry node that contains no code.
      std::shared_ptr< BasicGroup > entry = std::make_shared< BasicGroup >();
      basic_groups_.push_back( entry );
      result_func->entry_     = entry.get();
      size_t first_non_alloca = 0;
      for ( size_t i = 0; i < source_codes_.size(); ++i ) {
        auto code = source_codes_.at( i );
        if ( code_to_index_.find( code.get() ) != code_to_index_.end() ) {
          throw std::runtime_error( "Code already exists in Function." );
        }
        code_to_index_[ code.get() ] = i;
        if ( code->kind_ == SiiIRCodeKind::ALLOCA ) {
          if ( i != first_non_alloca ) {
            throw std::runtime_error(
                "Alloca must be continuous on the head of codes" );
          }
          first_non_alloca = i + 1;
          result_func->entry_->codes_.push_back( code );
        }
      }

      if ( first_non_alloca != source_codes_.size() ) {
        LabelPtr first_label = nullptr;
        if ( source_codes_[ first_non_alloca ]->label_ != nullptr ) {
          first_label = source_codes_[ first_non_alloca ]->label_;
        } else {
          first_label             = std::make_shared< Label >();
          first_label->dest_code_ = source_codes_[ first_non_alloca ].get();
          source_codes_[ first_non_alloca ]->label_ = first_label;
        }
        result_func->entry_->codes_.push_back(
            std::make_shared< SiiIRGoto >( first_label ) );
        auto follow = build_basic_group_starting_from( first_non_alloca );
        entry->follows_.push_back( follow );
        follow->precedes_.push_back( entry.get() );
      }

      // Set label_ field in basic groups.
      result_func->entry_->label_ = std::make_shared< Label >();
      for ( size_t i = 1; i < basic_groups_.size(); ++i ) {
        if ( basic_groups_[ i ]->label_ == nullptr ) {
          if ( basic_groups_[ i ]->codes_.begin()->label_ == nullptr ) {
            throw std::runtime_error(
                "Label is not set on the first code of a basic group" );
          }
          basic_groups_[ i ]->label_
              = basic_groups_[ i ]->codes_.begin()->label_;
          basic_groups_[ i ]->codes_.begin()->label_ = nullptr;
        }
      }

      // Set group_ field in codes.
      for ( size_t i = 0; i < basic_groups_.size(); ++i ) {
        for ( auto iter = basic_groups_[ i ]->codes_.begin();
              iter != basic_groups_[ i ]->codes_.end();
              ++iter ) {
          SiiIRCode& code = *iter;
          iter->group_    = basic_groups_[ i ].get();
        }
      }

      result_func->basic_groups_ = std::move( basic_groups_ );
      result_func->ctx_          = std::move( ctx_ );
      result_func->name_         = std::move( name );
      return result_func;
    }
  };

  FunctionPtr BuildFunction( std::vector< SiiIRCodePtr > codes,
                             FunctionContextPtr          ctx,
                             std::string                 name ) {
    FunctionBuilder builder( std::move( codes ), std::move( ctx ) );
    return builder.build( std::move( name ) );
  }

  std::string BasicGroup::to_string( IDAllocator& id_allocator ) const {
    std::stringstream result;
    result << label_->to_string( id_allocator ) << ":          ; pred: ";
    for ( size_t i = 0; i < precedes_.size(); ++i ) {
      result << precedes_[ i ]->label_->to_string( id_allocator );
      if ( i != precedes_.size() - 1 ) {
        result << ", ";
      } else {
        result << ";";
      }
    }
    result << std::endl;
    for ( auto iter = codes_.begin(); iter != codes_.end(); ++iter ) {
      result << iter->to_string( id_allocator ) << std::endl;
    }
    return result.str();
  }

  static void TraversePrintFunction( BasicGroup*              current_group,
                                     std::set< BasicGroup* >& visited,
                                     std::stringstream&       result,
                                     IDAllocator&             id_allocator ) {
    if ( visited.find( current_group ) != visited.end() ) {
      return;
    }
    visited.insert( current_group );
    result << current_group->to_string( id_allocator ) << std::endl;
    for ( auto& follow : current_group->follows_ ) {
      TraversePrintFunction( follow, visited, result, id_allocator );
    }
  }

  std::string Function::to_string() const {
    std::stringstream       result;
    std::set< BasicGroup* > visited;
    IDAllocator             id_allocator;
    result << "Function " << name_ << std::endl;
    TraversePrintFunction( entry_, visited, result, id_allocator );
    return result.str();
  }

}  // namespace SiiIR
