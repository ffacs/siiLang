#include "IR/IR.h"
#include "IR/code_builder.h"
#include "IR/function_ctx.h"
#include <gtest/gtest.h>

namespace SiiIR {
  TEST( Use, BinaryOperation ) {
    FunctionContextPtr ctx = std::make_shared< FunctionContext >(
        Type::Function( Type::Integer( 8 ), {} ) );
    auto left  = std::make_shared< ConstantValue >( "1", Type::Integer( 8 ) );
    auto right = std::make_shared< ConstantValue >( "2", Type::Integer( 8 ) );
    auto code_builder = CreateCodeBuilder();
    auto add          = code_builder->append_add( left, right );

    ASSERT_EQ( SiiIRCodeKind::ADD, add->kind_ );
    EXPECT_EQ( left, add->lhs_->value_ );
    EXPECT_EQ( right, add->rhs_->value_ );
    EXPECT_EQ( left->users_.size(), 1 );
    EXPECT_EQ( left->users_.begin()->user_, add.get() );
    EXPECT_EQ( right->users_.size(), 1 );
    EXPECT_EQ( right->users_.begin()->user_, add.get() );
    EXPECT_EQ( add->users_.size(), 0 );

    auto left2  = std::make_shared< ConstantValue >( "3", Type::Integer( 8 ) );
    auto right2 = std::make_shared< ConstantValue >( "3", Type::Integer( 8 ) );
    EXPECT_EQ( left2->users_.size(), 0 );
    add->use_setter< 0 >()( left2 );
    add->use_setter< 1 >()( right2 );
    EXPECT_EQ( left2->users_.size(), 1 );
    EXPECT_EQ( left2->users_.begin()->user_, add.get() );
    EXPECT_EQ( right2->users_.size(), 1 );
    EXPECT_EQ( right2->users_.begin()->user_, add.get() );

    EXPECT_EQ( left->users_.size(), 0 );
    EXPECT_EQ( right->users_.size(), 0 );
  }

  TEST( Use, UnaryOperation ) {
    FunctionContextPtr ctx = std::make_shared< FunctionContext >(
        Type::Function( Type::Integer( 8 ), {} ) );
    auto operand = std::make_shared< ConstantValue >( "1", Type::Integer( 8 ) );
    auto code_builder = CreateCodeBuilder();
    auto neg          = code_builder->append_neg( operand );

    ASSERT_EQ( SiiIRCodeKind::NEG, neg->kind_ );
    EXPECT_EQ( operand, neg->operand_->value_ );
    EXPECT_EQ( operand->users_.size(), 1 );
    EXPECT_EQ( operand->users_.begin()->user_, neg.get() );
    EXPECT_EQ( neg->users_.size(), 0 );

    auto operand2
        = std::make_shared< ConstantValue >( "1", Type::Integer( 8 ) );
    EXPECT_EQ( operand2->users_.size(), 0 );
    neg->use_setter< 0 >()( operand2 );
    EXPECT_EQ( operand2->users_.size(), 1 );
    EXPECT_EQ( operand2->users_.begin()->user_, neg.get() );
    EXPECT_EQ( neg->users_.size(), 0 );

    EXPECT_EQ( operand->users_.size(), 0 );
  }

  TEST( Use, Goto ) {
    FunctionContextPtr ctx = std::make_shared< FunctionContext >(
        Type::Function( Type::Integer( 8 ), {} ) );
    auto label        = std::make_shared< Label >();
    auto code_builder = CreateCodeBuilder();
    auto goto_code    = code_builder->append_goto( label );

    ASSERT_EQ( SiiIRCodeKind::GOTO, goto_code->kind_ );
    EXPECT_EQ( label, goto_code->dest_label_->value_ );
    EXPECT_EQ( label->users_.size(), 1 );
    EXPECT_EQ( label->users_.begin()->user_, goto_code.get() );

    auto label2 = std::make_shared< Label >();
    EXPECT_EQ( label2->users_.size(), 0 );
    goto_code->use_setter< 0 >()( label2 );
    EXPECT_EQ( label2->users_.size(), 1 );
    EXPECT_EQ( label2->users_.begin()->user_, goto_code.get() );

    EXPECT_EQ( label->users_.size(), 0 );
  }

  TEST( Use, ConditionBranch ) {
    // Arrange
    FunctionContextPtr ctx = std::make_shared< FunctionContext >(
        Type::Function( Type::Integer( 8 ), {} ) );
    auto condition
        = std::make_shared< ConstantValue >( "1", Type::Integer( 1 ) );
    auto true_label   = std::make_shared< Label >();
    auto false_label  = std::make_shared< Label >();
    auto code_builder = CreateCodeBuilder();

    auto cond_branch = code_builder->append_condition_branch(
        condition, true_label, false_label );

    ASSERT_EQ( SiiIRCodeKind::CONDITION_BRANCH, cond_branch->kind_ );

    EXPECT_EQ( condition, cond_branch->condition_->value_ );
    EXPECT_EQ( true_label, cond_branch->true_label_->value_ );
    EXPECT_EQ( false_label, cond_branch->false_label_->value_ );

    EXPECT_EQ( condition->users_.size(), 1 );
    EXPECT_EQ( condition->users_.begin()->user_, cond_branch.get() );
    EXPECT_EQ( true_label->users_.size(), 1 );
    EXPECT_EQ( true_label->users_.begin()->user_, cond_branch.get() );
    EXPECT_EQ( false_label->users_.size(), 1 );
    EXPECT_EQ( false_label->users_.begin()->user_, cond_branch.get() );

    auto new_condition
        = std::make_shared< ConstantValue >( "1", Type::Integer( 1 ) );
    auto new_true_label  = std::make_shared< Label >();
    auto new_false_label = std::make_shared< Label >();

    EXPECT_EQ( new_condition->users_.size(), 0 );
    EXPECT_EQ( new_true_label->users_.size(), 0 );
    EXPECT_EQ( new_false_label->users_.size(), 0 );

    cond_branch->use_setter< 0 >()( new_condition );
    cond_branch->use_setter< 1 >()( new_true_label );
    cond_branch->use_setter< 2 >()( new_false_label );

    EXPECT_EQ( new_condition->users_.size(), 1 );
    EXPECT_EQ( new_condition->users_.begin()->user_, cond_branch.get() );
    EXPECT_EQ( new_true_label->users_.size(), 1 );
    EXPECT_EQ( new_true_label->users_.begin()->user_, cond_branch.get() );
    EXPECT_EQ( new_false_label->users_.size(), 1 );
    EXPECT_EQ( new_false_label->users_.begin()->user_, cond_branch.get() );

    EXPECT_EQ( condition->users_.size(), 0 );
    EXPECT_EQ( true_label->users_.size(), 0 );
    EXPECT_EQ( false_label->users_.size(), 0 );
  }

  TEST( Use, Load ) {
    FunctionContextPtr ctx = std::make_shared< FunctionContext >(
        Type::Function( Type::Integer( 8 ), {} ) );
    auto src = std::make_shared< ConstantValue >(
        "1", Type::Pointer( Type::Integer( 8 ) ) );
    auto code_builder = CreateCodeBuilder();

    auto load = code_builder->append_load( src );

    ASSERT_EQ( SiiIRCodeKind::LOAD, load->kind_ );

    EXPECT_EQ( src, load->src_->value_ );

    EXPECT_EQ( src->users_.size(), 1 );
    EXPECT_EQ( src->users_.begin()->user_, load.get() );

    auto new_src = std::make_shared< ConstantValue >(
        "1", Type::Pointer( Type::Integer( 8 ) ) );
    EXPECT_EQ( new_src->users_.size(), 0 );

    load->use_setter< 0 >()( new_src );
    EXPECT_EQ( new_src->users_.size(), 1 );
    EXPECT_EQ( new_src->users_.begin()->user_, load.get() );

    EXPECT_EQ( src->users_.size(), 0 );
  }

  TEST( Use, Store ) {
    FunctionContextPtr ctx = std::make_shared< FunctionContext >(
        Type::Function( Type::Integer( 8 ), {} ) );
    auto src  = std::make_shared< ConstantValue >( "1", Type::Integer( 8 ) );
    auto dest = std::make_shared< ConstantValue >(
        "1", Type::Pointer( Type::Integer( 8 ) ) );
    auto code_builder = CreateCodeBuilder();

    auto store = code_builder->append_store( src, dest );

    EXPECT_EQ( src, store->src_->value_ );
    EXPECT_EQ( dest, store->dest_->value_ );

    EXPECT_EQ( src->users_.size(), 1 );
    EXPECT_EQ( src->users_.begin()->user_, store.get() );
    EXPECT_EQ( dest->users_.size(), 1 );
    EXPECT_EQ( dest->users_.begin()->user_, store.get() );

    auto new_src = std::make_shared< ConstantValue >( "1", Type::Integer( 8 ) );
    auto new_dest = std::make_shared< ConstantValue >(
        "1", Type::Pointer( Type::Integer( 8 ) ) );
    EXPECT_EQ( new_src->users_.size(), 0 );
    EXPECT_EQ( new_dest->users_.size(), 0 );

    store->use_setter< 0 >()( new_src );
    store->use_setter< 1 >()( new_dest );

    EXPECT_EQ( new_src->users_.size(), 1 );
    EXPECT_EQ( new_src->users_.begin()->user_, store.get() );
    EXPECT_EQ( new_dest->users_.size(), 1 );
    EXPECT_EQ( new_dest->users_.begin()->user_, store.get() );

    EXPECT_EQ( src->users_.size(), 0 );
    EXPECT_EQ( dest->users_.size(), 0 );
  }

  TEST( Use, Phi ) {
    // Arrange
    FunctionContextPtr ctx = std::make_shared< FunctionContext >(
        Type::Function( Type::Integer( 8 ), {} ) );
    size_t src_size = 3;

    // Act
    auto address = std::make_shared< ConstantValue >(
        "1", Type::Pointer( Type::Integer( 8 ) ) );
    auto phi = std::make_shared< SiiIRPhi >( address, src_size );

    EXPECT_EQ( src_size, phi->src_list_.size() );
    EXPECT_EQ( address, phi->src_list_[ 0 ]->value_ );
    EXPECT_EQ( address, phi->src_list_[ 1 ]->value_ );
    EXPECT_EQ( address, phi->src_list_[ 2 ]->value_ );

    EXPECT_EQ( address->users_.size(), 3 );

    auto new_src1
        = std::make_shared< ConstantValue >( "1", Type::Integer( 8 ) );
    auto new_src2
        = std::make_shared< ConstantValue >( "1", Type::Integer( 8 ) );
    auto new_src3
        = std::make_shared< ConstantValue >( "1", Type::Integer( 8 ) );
    EXPECT_EQ( new_src1->users_.size(), 0 );
    EXPECT_EQ( new_src2->users_.size(), 0 );
    EXPECT_EQ( new_src3->users_.size(), 0 );

    phi->replace_src( 0, new_src1 );
    phi->replace_src( 1, new_src2 );
    phi->replace_src( 2, new_src3 );

    EXPECT_EQ( new_src1->users_.size(), 1 );
    EXPECT_EQ( new_src1->users_.begin()->user_, phi.get() );
    EXPECT_EQ( new_src2->users_.size(), 1 );
    EXPECT_EQ( new_src2->users_.begin()->user_, phi.get() );
    EXPECT_EQ( new_src3->users_.size(), 1 );
    EXPECT_EQ( new_src3->users_.begin()->user_, phi.get() );
  }
}  // namespace SiiIR
