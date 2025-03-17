#include "IR/code_builder.h"
#include "IR/function.h"
#include <gtest/gtest.h>

namespace SiiIR {
TEST(Use, BinaryOperation) {
  FunctionContextPtr ctx =
      std::make_shared<FunctionContext>(Type::Function(Type::Integer(8), {}));
  auto left = ctx->allocate_temporary_value(Type::Integer(8));
  auto right = ctx->allocate_temporary_value(Type::Integer(8));
  auto result = ctx->allocate_temporary_value(Type::Integer(8));
  auto code_builder = CreateCodeBuilder();
  code_builder->append_add(left, right, result);
  auto result_codes_ptr = code_builder->finish();
  auto &final_codes = *result_codes_ptr;
  ASSERT_EQ(1, final_codes.size());

  ASSERT_EQ(SiiIRCodeKind::ADD, final_codes[0]->kind_);
  EXPECT_EQ(left, std::static_pointer_cast<SiiIRBinaryOperation>(final_codes[0])
                      ->lhs_->value_);
  EXPECT_EQ(right,
            std::static_pointer_cast<SiiIRBinaryOperation>(final_codes[0])
                ->rhs_->value_);
  EXPECT_EQ(result,
            std::static_pointer_cast<SiiIRBinaryOperation>(final_codes[0])
                ->result_->value_);
  EXPECT_EQ(left->users_.size(), 1);
  EXPECT_EQ(left->users_.begin()->user_, final_codes[0].get());
  EXPECT_EQ(right->users_.size(), 1);
  EXPECT_EQ(right->users_.begin()->user_, final_codes[0].get());
  EXPECT_EQ(result->users_.size(), 1);
  EXPECT_EQ(result->users_.begin()->user_, final_codes[0].get());

  auto left2 = ctx->allocate_temporary_value(Type::Integer(8));
  EXPECT_EQ(left2->users_.size(), 0);
  std::static_pointer_cast<SiiIRBinaryOperation>(final_codes[0])
      ->use_setter<0>()(left2);
  EXPECT_EQ(left2->users_.size(), 1);
  EXPECT_EQ(left2->users_.begin()->user_, final_codes[0].get());
  EXPECT_EQ(right->users_.size(), 1);
  EXPECT_EQ(right->users_.begin()->user_, final_codes[0].get());
  EXPECT_EQ(result->users_.size(), 1);
  EXPECT_EQ(result->users_.begin()->user_, final_codes[0].get());

  EXPECT_EQ(left->users_.size(), 0);
}

TEST(Use, UnaryOperation) {
  FunctionContextPtr ctx =
      std::make_shared<FunctionContext>(Type::Function(Type::Integer(8), {}));
  auto operand = ctx->allocate_temporary_value(Type::Integer(8));
  auto result = ctx->allocate_temporary_value(Type::Integer(8));
  auto code_builder = CreateCodeBuilder();
  code_builder->append_neg(operand, result);
  auto result_codes_ptr = code_builder->finish();
  auto &final_codes = *result_codes_ptr;
  ASSERT_EQ(1, final_codes.size());

  ASSERT_EQ(SiiIRCodeKind::NEG, final_codes[0]->kind_);
  EXPECT_EQ(operand,
            std::static_pointer_cast<SiiIRUnaryOperation>(final_codes[0])
                ->operand_->value_);
  EXPECT_EQ(result,
            std::static_pointer_cast<SiiIRUnaryOperation>(final_codes[0])
                ->result_->value_);
  EXPECT_EQ(operand->users_.size(), 1);
  EXPECT_EQ(operand->users_.begin()->user_, final_codes[0].get());
  EXPECT_EQ(result->users_.size(), 1);
  EXPECT_EQ(result->users_.begin()->user_, final_codes[0].get());

  auto operand2 = ctx->allocate_temporary_value(Type::Integer(8));
  EXPECT_EQ(operand2->users_.size(), 0);
  std::static_pointer_cast<SiiIRUnaryOperation>(final_codes[0])
      ->use_setter<0>()(operand2);
  EXPECT_EQ(operand2->users_.size(), 1);
  EXPECT_EQ(operand2->users_.begin()->user_, final_codes[0].get());
  EXPECT_EQ(result->users_.size(), 1);
  EXPECT_EQ(result->users_.begin()->user_, final_codes[0].get());

  EXPECT_EQ(operand->users_.size(), 0);
}

TEST(Use, Goto) {
  FunctionContextPtr ctx =
      std::make_shared<FunctionContext>(Type::Function(Type::Integer(8), {}));
  auto label = ctx->allocate_label();
  auto code_builder = CreateCodeBuilder();
  code_builder->append_goto(label);
  auto result_codes_ptr = code_builder->finish();
  auto &final_codes = *result_codes_ptr;
  ASSERT_EQ(1, final_codes.size());

  ASSERT_EQ(SiiIRCodeKind::GOTO, final_codes[0]->kind_);
  EXPECT_EQ(
      label,
      std::static_pointer_cast<SiiIRGoto>(final_codes[0])->dest_label_->value_);
  EXPECT_EQ(label->users_.size(), 1);
  EXPECT_EQ(label->users_.begin()->user_, final_codes[0].get());

  auto label2 = ctx->allocate_label();
  EXPECT_EQ(label2->users_.size(), 0);
  std::static_pointer_cast<SiiIRGoto>(final_codes[0])->use_setter<0>()(label2);
  EXPECT_EQ(label2->users_.size(), 1);
  EXPECT_EQ(label2->users_.begin()->user_, final_codes[0].get());

  EXPECT_EQ(label->users_.size(), 0);
}

TEST(Use, ConditionBranch) {
  // Arrange
  FunctionContextPtr ctx =
      std::make_shared<FunctionContext>(Type::Function(Type::Integer(8), {}));
  auto condition = ctx->allocate_temporary_value(Type::Integer(1));
  auto true_label = ctx->allocate_label();
  auto false_label = ctx->allocate_label();
  auto code_builder = CreateCodeBuilder();

  code_builder->append_condition_branch(condition, true_label, false_label);
  auto result_codes_ptr = code_builder->finish();
  auto &final_codes = *result_codes_ptr;

  ASSERT_EQ(1, final_codes.size());
  ASSERT_EQ(SiiIRCodeKind::CONDITION_BRANCH, final_codes[0]->kind_);

  EXPECT_EQ(condition,
            std::static_pointer_cast<SiiIRConditionBranch>(final_codes[0])
                ->condition_->value_);
  EXPECT_EQ(true_label,
            std::static_pointer_cast<SiiIRConditionBranch>(final_codes[0])
                ->true_label_->value_);
  EXPECT_EQ(false_label,
            std::static_pointer_cast<SiiIRConditionBranch>(final_codes[0])
                ->false_label_->value_);

  EXPECT_EQ(condition->users_.size(), 1);
  EXPECT_EQ(condition->users_.begin()->user_, final_codes[0].get());
  EXPECT_EQ(true_label->users_.size(), 1);
  EXPECT_EQ(true_label->users_.begin()->user_, final_codes[0].get());
  EXPECT_EQ(false_label->users_.size(), 1);
  EXPECT_EQ(false_label->users_.begin()->user_, final_codes[0].get());

  auto new_condition = ctx->allocate_temporary_value(Type::Integer(1));
  auto new_true_label = ctx->allocate_label();
  auto new_false_label = ctx->allocate_label();

  EXPECT_EQ(new_condition->users_.size(), 0);
  EXPECT_EQ(new_true_label->users_.size(), 0);
  EXPECT_EQ(new_false_label->users_.size(), 0);

  std::static_pointer_cast<SiiIRConditionBranch>(final_codes[0])
      ->use_setter<0>()(new_condition);
  std::static_pointer_cast<SiiIRConditionBranch>(final_codes[0])
      ->use_setter<1>()(new_true_label);
  std::static_pointer_cast<SiiIRConditionBranch>(final_codes[0])
      ->use_setter<2>()(new_false_label);

  EXPECT_EQ(new_condition->users_.size(), 1);
  EXPECT_EQ(new_condition->users_.begin()->user_, final_codes[0].get());
  EXPECT_EQ(new_true_label->users_.size(), 1);
  EXPECT_EQ(new_true_label->users_.begin()->user_, final_codes[0].get());
  EXPECT_EQ(new_false_label->users_.size(), 1);
  EXPECT_EQ(new_false_label->users_.begin()->user_, final_codes[0].get());

  EXPECT_EQ(condition->users_.size(), 0);
  EXPECT_EQ(true_label->users_.size(), 0);
  EXPECT_EQ(false_label->users_.size(), 0);
}

TEST(Use, Alloca) {
  FunctionContextPtr ctx =
      std::make_shared<FunctionContext>(Type::Function(Type::Integer(8), {}));
  auto dest = ctx->allocate_variable_value(Type::Integer(8));
  uint32_t size = 10;
  auto code_builder = CreateCodeBuilder();

  code_builder->append_alloca(dest, size);
  auto result_codes_ptr = code_builder->finish();
  auto &final_codes = *result_codes_ptr;

  ASSERT_EQ(1, final_codes.size());
  ASSERT_EQ(SiiIRCodeKind::ALLOCA, final_codes[0]->kind_);

  EXPECT_EQ(
      dest,
      std::static_pointer_cast<SiiIRAlloca>(final_codes[0])->dest_->value_);
  EXPECT_EQ(size, std::static_pointer_cast<SiiIRAlloca>(final_codes[0])->size_);

  EXPECT_EQ(dest->users_.size(), 1);
  EXPECT_EQ(dest->users_.begin()->user_, final_codes[0].get());

  auto new_dest = ctx->allocate_variable_value(Type::Integer(8));
  EXPECT_EQ(new_dest->users_.size(), 0);

  std::static_pointer_cast<SiiIRAlloca>(final_codes[0])
      ->use_setter<0>()(new_dest);

  EXPECT_EQ(new_dest->users_.size(), 1);
  EXPECT_EQ(new_dest->users_.begin()->user_, final_codes[0].get());

  EXPECT_EQ(dest->users_.size(), 0);
}

TEST(Use, Load) {
  FunctionContextPtr ctx =
      std::make_shared<FunctionContext>(Type::Function(Type::Integer(8), {}));
  auto src = ctx->allocate_variable_value(Type::Integer(8));
  auto dest = ctx->allocate_temporary_value(Type::Integer(8));
  auto code_builder = CreateCodeBuilder();

  code_builder->append_load(src, dest);
  auto result_codes_ptr = code_builder->finish();
  auto &final_codes = *result_codes_ptr;

  ASSERT_EQ(1, final_codes.size());
  ASSERT_EQ(SiiIRCodeKind::LOAD, final_codes[0]->kind_);

  EXPECT_EQ(src,
            std::static_pointer_cast<SiiIRLoad>(final_codes[0])->src_->value_);
  EXPECT_EQ(dest,
            std::static_pointer_cast<SiiIRLoad>(final_codes[0])->dest_->value_);

  EXPECT_EQ(src->users_.size(), 1);
  EXPECT_EQ(src->users_.begin()->user_, final_codes[0].get());
  EXPECT_EQ(dest->users_.size(), 1);
  EXPECT_EQ(dest->users_.begin()->user_, final_codes[0].get());

  auto new_src = ctx->allocate_variable_value(Type::Integer(8));
  auto new_dest = ctx->allocate_temporary_value(Type::Integer(8));
  EXPECT_EQ(new_src->users_.size(), 0);
  EXPECT_EQ(new_dest->users_.size(), 0);

  std::static_pointer_cast<SiiIRLoad>(final_codes[0])->use_setter<0>()(new_src);
  std::static_pointer_cast<SiiIRLoad>(final_codes[0])
      ->use_setter<1>()(new_dest);

  EXPECT_EQ(new_src->users_.size(), 1);
  EXPECT_EQ(new_src->users_.begin()->user_, final_codes[0].get());
  EXPECT_EQ(new_dest->users_.size(), 1);
  EXPECT_EQ(new_dest->users_.begin()->user_, final_codes[0].get());

  EXPECT_EQ(src->users_.size(), 0);
  EXPECT_EQ(dest->users_.size(), 0);
}

TEST(Use, Store) {
  FunctionContextPtr ctx =
      std::make_shared<FunctionContext>(Type::Function(Type::Integer(8), {}));
  auto src = ctx->allocate_temporary_value(Type::Integer(8));
  auto dest = ctx->allocate_variable_value(Type::Integer(8));
  auto code_builder = CreateCodeBuilder();

  code_builder->append_store(src, dest);
  auto result_codes_ptr = code_builder->finish();
  auto &final_codes = *result_codes_ptr;

  ASSERT_EQ(1, final_codes.size());
  ASSERT_EQ(SiiIRCodeKind::STORE, final_codes[0]->kind_);

  EXPECT_EQ(src,
            std::static_pointer_cast<SiiIRStore>(final_codes[0])->src_->value_);
  EXPECT_EQ(
      dest,
      std::static_pointer_cast<SiiIRStore>(final_codes[0])->dest_->value_);

  EXPECT_EQ(src->users_.size(), 1);
  EXPECT_EQ(src->users_.begin()->user_, final_codes[0].get());
  EXPECT_EQ(dest->users_.size(), 1);
  EXPECT_EQ(dest->users_.begin()->user_, final_codes[0].get());

  auto new_src = ctx->allocate_temporary_value(Type::Integer(8));
  auto new_dest = ctx->allocate_variable_value(Type::Integer(8));
  EXPECT_EQ(new_src->users_.size(), 0);
  EXPECT_EQ(new_dest->users_.size(), 0);

  std::static_pointer_cast<SiiIRStore>(final_codes[0])
      ->use_setter<0>()(new_src);
  std::static_pointer_cast<SiiIRStore>(final_codes[0])
      ->use_setter<1>()(new_dest);

  EXPECT_EQ(new_src->users_.size(), 1);
  EXPECT_EQ(new_src->users_.begin()->user_, final_codes[0].get());
  EXPECT_EQ(new_dest->users_.size(), 1);
  EXPECT_EQ(new_dest->users_.begin()->user_, final_codes[0].get());

  EXPECT_EQ(src->users_.size(), 0);
  EXPECT_EQ(dest->users_.size(), 0);
}

TEST(Use, Phi) {
  // Arrange
  FunctionContextPtr ctx =
      std::make_shared<FunctionContext>(Type::Function(Type::Integer(8), {}));
  auto dest = ctx->allocate_variable_value(Type::Integer(8));
  size_t src_size = 3;

  // Act
  auto phi = std::make_shared<SiiIRPhi>(dest, src_size);

  EXPECT_EQ(dest, phi->dest_->value_);
  EXPECT_EQ(src_size, phi->src_list_.size());
  EXPECT_EQ(dest, phi->src_list_[0]->value_);
  EXPECT_EQ(dest, phi->src_list_[1]->value_);
  EXPECT_EQ(dest, phi->src_list_[2]->value_);

  EXPECT_EQ(dest->users_.size(), 4);
  for (auto it = dest->users_.begin(); it != dest->users_.end(); ++it) {
    EXPECT_EQ(it->user_, phi.get());
  }

  auto new_dest = ctx->allocate_variable_value(Type::Integer(8));
  EXPECT_EQ(new_dest->users_.size(), 0);

  phi->use_setter<0>()(new_dest);

  EXPECT_EQ(new_dest->users_.size(), 1);
  EXPECT_EQ(new_dest->users_.begin()->user_, phi.get());

  EXPECT_EQ(dest->users_.size(), 3);
  for (auto it = dest->users_.begin(); it != dest->users_.end(); ++it) {
    EXPECT_EQ(it->user_, phi.get());
  }

  auto new_src1 = ctx->allocate_temporary_value(Type::Integer(8));
  auto new_src2 = ctx->allocate_temporary_value(Type::Integer(8));
  auto new_src3 = ctx->allocate_temporary_value(Type::Integer(8));
  EXPECT_EQ(new_src1->users_.size(), 0);
  EXPECT_EQ(new_src2->users_.size(), 0);
  EXPECT_EQ(new_src3->users_.size(), 0);

  phi->replace_src(0, new_src1);
  phi->replace_src(1, new_src2);
  phi->replace_src(2, new_src3);

  EXPECT_EQ(new_src1->users_.size(), 1);
  EXPECT_EQ(new_src1->users_.begin()->user_, phi.get());
  EXPECT_EQ(new_src2->users_.size(), 1);
  EXPECT_EQ(new_src2->users_.begin()->user_, phi.get());
  EXPECT_EQ(new_src3->users_.size(), 1);
  EXPECT_EQ(new_src3->users_.begin()->user_, phi.get());

  EXPECT_EQ(dest->users_.size(), 0);
}

} // namespace SiiIR