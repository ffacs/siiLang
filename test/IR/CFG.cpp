#include "IR/CFG.h"
#include "IR/code_builder.h"
#include "gtest/gtest.h"

namespace SiiIR {
TEST(CFG, BuildCFGFromEmptyCodes) {
  auto codes = std::make_shared<std::vector<SiiIRCodePtr>>();
  auto cfg = BuildCFG(codes);
  EXPECT_EQ(cfg->codes_in_order_, codes);
  EXPECT_EQ(cfg->basic_groups_.size(), 1);
  ASSERT_NE(cfg->entry_, nullptr);
  EXPECT_EQ(cfg->entry_->follows_.size(), 0);
}

TEST(CFG, BuildSingleBasicGroup) {
  auto code_builder = CreateCodeBuilder();
  code_builder->append_nope();
  code_builder->append_nope();
  code_builder->append_nope();
  code_builder->append_nope();
  auto codes = code_builder->finish();
  auto cfg = BuildCFG(codes);
  EXPECT_EQ(cfg->codes_in_order_, codes);
  EXPECT_EQ(cfg->basic_groups_.size(), 2);
  ASSERT_NE(cfg->entry_, nullptr);
  EXPECT_EQ(cfg->entry_->follows_.size(), 1);
  EXPECT_EQ(cfg->entry_->follows_[0], cfg->basic_groups_[1].get());
  auto group1 = cfg->basic_groups_[1];
  EXPECT_EQ(group1->basic_group_->codes_.size(), 4LL);
  EXPECT_EQ(group1->basic_group_->codes_[0], codes->at(0).get());
  EXPECT_EQ(group1->basic_group_->codes_[1], codes->at(1).get());
  EXPECT_EQ(group1->basic_group_->codes_[2], codes->at(2).get());
  EXPECT_EQ(group1->basic_group_->codes_[3], codes->at(3).get());
}

TEST(CFG, BuildCFGWithLabel) {
  auto code_builder = CreateCodeBuilder();
  code_builder->append_label(code_builder->new_label());
  code_builder->append_nope();
  auto codes = code_builder->finish();
  auto cfg = BuildCFG(codes);
  EXPECT_EQ(cfg->codes_in_order_, codes);
  EXPECT_EQ(cfg->basic_groups_.size(), 2);
  ASSERT_NE(cfg->entry_, nullptr);
  auto entry = cfg->entry_;
  EXPECT_EQ(entry->follows_.size(), 1);
  EXPECT_EQ(entry->follows_[0], cfg->basic_groups_[1].get());
  auto group1 = cfg->basic_groups_[1];
  EXPECT_EQ(group1->basic_group_->codes_.size(), 1LL);
  EXPECT_EQ(group1->basic_group_->codes_[0], codes->at(0).get());
}

TEST(CFG, BuildCFGWithGoto) {
  auto code_builder = CreateCodeBuilder();
  auto label1 = code_builder->new_label();
  code_builder->append_nope();
  code_builder->append_label(label1);
  code_builder->append_nope();
  code_builder->append_nope();
  code_builder->append_goto(label1);
  auto codes = code_builder->finish();
  auto cfg = BuildCFG(codes);
  EXPECT_EQ(cfg->codes_in_order_, codes);
  EXPECT_EQ(cfg->basic_groups_.size(), 3);
  auto entry = cfg->entry_;
  EXPECT_EQ(entry->follows_.size(), 1);
  auto group1 = entry->follows_[0];
  EXPECT_EQ(group1->basic_group_->codes_.size(), 1LL);
  EXPECT_EQ(group1->basic_group_->codes_[0], codes->at(0).get());
  EXPECT_EQ(group1->follows_.size(), 1);
  auto group2 = group1->follows_[0];
  EXPECT_EQ(group2->basic_group_->codes_.size(), 3LL);
  EXPECT_EQ(group2->basic_group_->codes_[0], codes->at(1).get());
  EXPECT_EQ(group2->basic_group_->codes_[1], codes->at(2).get());
  EXPECT_EQ(group2->basic_group_->codes_[2], codes->at(3).get());
  EXPECT_EQ(group2->follows_.size(), 1);
  EXPECT_EQ(group2->follows_[0], group2);
}

TEST(CFG, BuildCFGWithIfTrueGoto) {
  auto code_builder = CreateCodeBuilder();
  AddressPtr expression = Address::constant("constant");
  auto label1 = code_builder->new_label();
  // group1
  code_builder->append_nope();
  code_builder->append_if_true_goto(expression, label1);
  // group3
  code_builder->append_nope();
  code_builder->append_nope();
  // group2
  code_builder->append_label(label1);
  code_builder->append_nope();
  auto codes = code_builder->finish();
  auto cfg = BuildCFG(codes);
  EXPECT_EQ(cfg->codes_in_order_, codes);
  EXPECT_EQ(cfg->basic_groups_.size(), 4);
  auto entry = cfg->entry_;
  EXPECT_EQ(entry->follows_.size(), 1);
  auto group1 = entry->follows_[0];
  EXPECT_EQ(group1->basic_group_->codes_.size(), 2LL);
  EXPECT_EQ(group1->basic_group_->codes_[0], codes->at(0).get());
  EXPECT_EQ(group1->basic_group_->codes_[1], codes->at(1).get());
  EXPECT_EQ(group1->follows_.size(), 2);

  auto group2 = group1->follows_[0];
  EXPECT_EQ(group2->basic_group_->codes_.size(), 1LL);
  EXPECT_EQ(group2->basic_group_->codes_[0], codes->at(4).get());
  EXPECT_EQ(group2->follows_.size(), 0);
  
  auto group3 = group1->follows_[1];
  EXPECT_EQ(group3->basic_group_->codes_.size(), 2LL);
  EXPECT_EQ(group3->basic_group_->codes_[0], codes->at(2).get());
  EXPECT_EQ(group3->basic_group_->codes_[1], codes->at(3).get());
  EXPECT_EQ(group3->follows_.size(), 1);

}

} // namespace SiiIR