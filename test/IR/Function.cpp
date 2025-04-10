#include "IR/function.h"
#include "IR/code_builder.h"
#include <gtest/gtest.h>

namespace SiiIR {

TEST(Function, BuildFunctionFromEmptyCodes) {
  FunctionContextPtr ctx =
      std::make_shared<FunctionContext>(Type::Function(Type::Integer(8), {}));
  auto codes = std::make_shared<std::vector<SiiIRCodePtr>>();
  auto func = BuildFunction(*codes, ctx, "");
  EXPECT_EQ(func->basic_groups_.size(), 1);
  ASSERT_NE(func->entry_, nullptr);
  EXPECT_EQ(func->entry_->follows_.size(), 0);
}

TEST(Function, BuildSingleBasicGroup) {
  FunctionContextPtr ctx =
      std::make_shared<FunctionContext>(Type::Function(Type::Integer(8), {}));
  auto code_builder = CreateCodeBuilder();
  code_builder->append_nope();
  code_builder->append_nope();
  code_builder->append_nope();
  code_builder->append_nope();
  auto codes = code_builder->finish();
  auto func = BuildFunction(*codes, ctx, "");
  EXPECT_EQ(func->basic_groups_.size(), 2);
  ASSERT_NE(func->entry_, nullptr);
  EXPECT_EQ(func->entry_->follows_.size(), 1);
  EXPECT_EQ(func->entry_->follows_[0], func->basic_groups_[1].get());
  auto group1 = func->basic_groups_[1];
  EXPECT_EQ(group1->codes_.size(), 4LL);
  EXPECT_EQ(&group1->codes_[0], codes->at(0).get());
  EXPECT_EQ(&group1->codes_[1], codes->at(1).get());
  EXPECT_EQ(&group1->codes_[2], codes->at(2).get());
  EXPECT_EQ(&group1->codes_[3], codes->at(3).get());
}

TEST(Function, BuildFunctionWithLabel) {
  FunctionContextPtr ctx =
      std::make_shared<FunctionContext>(Type::Function(Type::Integer(8), {}));
  auto code_builder = CreateCodeBuilder();
  code_builder->append_label(std::make_shared<Label>());
  code_builder->append_nope();
  auto codes = code_builder->finish();
  auto func = BuildFunction(*codes, ctx, "");
  EXPECT_EQ(func->basic_groups_.size(), 2);
  ASSERT_NE(func->entry_, nullptr);
  auto entry = func->entry_;
  EXPECT_EQ(entry->follows_.size(), 1);
  EXPECT_EQ(entry->follows_[0], func->basic_groups_[1].get());
  auto group1 = func->basic_groups_[1];
  EXPECT_EQ(group1->codes_.size(), 1LL);
  EXPECT_EQ(&group1->codes_[0], codes->at(0).get());
}

TEST(Function, BuildFunctionWithGoto) {
  FunctionContextPtr ctx =
      std::make_shared<FunctionContext>(Type::Function(Type::Integer(8), {}));
  auto code_builder = CreateCodeBuilder();
  auto label1 = std::make_shared<Label>();
  code_builder->append_nope();
  code_builder->append_label(label1);
  code_builder->append_nope();
  code_builder->append_nope();
  code_builder->append_goto(label1);
  auto codes = code_builder->finish();
  auto func = BuildFunction(*codes, ctx, "");
  EXPECT_EQ(func->basic_groups_.size(), 3);
  auto entry = func->entry_;
  EXPECT_EQ(entry->follows_.size(), 1);

  auto group1 = entry->follows_[0];
  EXPECT_EQ(group1->codes_.size(), 2LL);
  EXPECT_EQ(&group1->codes_[0], codes->at(0).get());
  EXPECT_EQ(group1->codes_[1].kind_, SiiIRCodeKind::GOTO);
  EXPECT_EQ(static_cast<SiiIRGoto &>(group1->codes_[1]).dest_label_->value_,
            label1);
  EXPECT_EQ(group1->follows_.size(), 1);

  auto group2 = group1->follows_[0];
  EXPECT_EQ(group2->codes_.size(), 3LL);
  EXPECT_EQ(&group2->codes_[0], codes->at(1).get());
  EXPECT_EQ(&group2->codes_[1], codes->at(2).get());
  EXPECT_EQ(&group2->codes_[2], codes->at(3).get());
  EXPECT_EQ(group2->follows_.size(), 1);
  EXPECT_EQ(group2->follows_[0], group2);
}

TEST(Function, BuildFunctionWithConditionBranch) {
  FunctionContextPtr ctx =
      std::make_shared<FunctionContext>(Type::Function(Type::Integer(8), {}));
  auto code_builder = CreateCodeBuilder();
  auto expression = Value::constant("constant", Type::Integer(1));
  auto true_label = std::make_shared<Label>();
  auto false_label = std::make_shared<Label>();
  // group1
  code_builder->append_nope();
  code_builder->append_condition_branch(expression, true_label, false_label);
  // group3
  code_builder->append_label(true_label);
  code_builder->append_nope();
  code_builder->append_nope();
  // group2
  code_builder->append_label(false_label);
  code_builder->append_nope();

  auto codes = code_builder->finish();
  auto func = BuildFunction(*codes, ctx, "");
  EXPECT_EQ(func->basic_groups_.size(), 4);
  auto entry = func->entry_;
  EXPECT_EQ(entry->follows_.size(), 1);
  auto group1 = entry->follows_[0];
  EXPECT_EQ(group1->codes_.size(), 2LL);
  EXPECT_EQ(&group1->codes_[0], codes->at(0).get());
  EXPECT_EQ(&group1->codes_[1], codes->at(1).get());
  EXPECT_EQ(group1->follows_.size(), 2);
  EXPECT_EQ(group1->precedes_.size(), 1);
  EXPECT_EQ(group1->precedes_[0], entry);

  auto group3 = group1->follows_[0];
  EXPECT_EQ(group3->codes_.size(), 3LL);
  EXPECT_EQ(&group3->codes_[0], codes->at(2).get());
  EXPECT_EQ(&group3->codes_[1], codes->at(3).get());
  EXPECT_EQ(group3->codes_[2].kind_, SiiIRCodeKind::GOTO);
  EXPECT_EQ(group3->follows_.size(), 1);
  EXPECT_EQ(group3->precedes_.size(), 1);
  EXPECT_EQ(group3->precedes_[0], group1);

  auto group2 = group1->follows_[1];
  EXPECT_EQ(group2->codes_.size(), 1LL);
  EXPECT_EQ(&group2->codes_[0], codes->at(4).get());
  EXPECT_EQ(group2->follows_.size(), 0);
  EXPECT_EQ(group2->precedes_.size(), 2);
  EXPECT_EQ(group2->precedes_[0], group3);
  EXPECT_EQ(group2->precedes_[1], group1);
}

TEST(Function, BuildFunctionWithAlloca) {
  FunctionContextPtr ctx =
      std::make_shared<FunctionContext>(Type::Function(Type::Integer(8), {}));
  auto code_builder = CreateCodeBuilder();
  auto address1 = code_builder->append_alloca(4, Type::Integer(8));
  code_builder->append_alloca(4, Type::Integer(8));
  code_builder->append_nope();
  auto codes = code_builder->finish();
  auto func = BuildFunction(*codes, ctx, "");
  EXPECT_EQ(func->basic_groups_.size(), 2);
  auto entry = func->entry_;
  EXPECT_EQ(entry->codes_.size(), 3);
  EXPECT_EQ(&entry->codes_[0], codes->at(0).get());
  EXPECT_EQ(&entry->codes_[1], codes->at(1).get());
  EXPECT_EQ(entry->codes_[2].kind_, SiiIRCodeKind::GOTO);
  EXPECT_EQ(entry->follows_.size(), 1);
  auto group1 = entry->follows_[0];
  EXPECT_EQ(group1->codes_.size(), 1LL);
  EXPECT_EQ(&group1->codes_[0], codes->at(2).get());
  EXPECT_EQ(group1->follows_.size(), 0);
}

} // namespace SiiIR