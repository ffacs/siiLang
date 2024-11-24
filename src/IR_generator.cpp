#include "IR_generator.h"
#include "code_builder.h"

class IRGeneratorImpl : public IRGenerator {
 public:
  IRGeneratorImpl(ASTNodePtr ast) : ast_(std::move(ast)) {
    code_builder_ = CreateCodeBuilder();
  }
  std::vector<ThreeAddressCodePtr> work() override;

protected:
  ASTNodePtr ast_;
  std::unique_ptr<CodeBuilder> code_builder_;

  AddressPtr generate_for_node(const ASTNodePtr& node);
  AddressPtr generate_for_mul_node(const ASTNodePtr& node);
  AddressPtr generate_for_div_node(const ASTNodePtr& node);
  AddressPtr generate_for_add_node(const ASTNodePtr& node);
  AddressPtr generate_for_sub_node(const ASTNodePtr& node);
  AddressPtr generate_for_neg_node(const ASTNodePtr& node);
  AddressPtr generate_for_equal_node(const ASTNodePtr& node);
  AddressPtr generate_for_not_equal_node(const ASTNodePtr& node);
  AddressPtr generate_for_less_than_node(const ASTNodePtr& node);
  AddressPtr generate_for_less_equal_node(const ASTNodePtr& node);
  AddressPtr generate_for_integer_node(const ASTNodePtr& node);
  AddressPtr generate_for_variable_node(const ASTNodePtr& node);
  AddressPtr generate_for_assign_node(const ASTNodePtr& node);
  AddressPtr generate_for_if_else_node(const ASTNodePtr& node);
  AddressPtr generate_for_compound_statement_node(const ASTNodePtr& node);
};

AddressPtr IRGeneratorImpl::generate_for_node(const ASTNodePtr& node) {
  switch (node->type_) {
    case ASTNodeType::MUL:
      return generate_for_mul_node(node);
    case ASTNodeType::DIV:
      return generate_for_div_node(node);
    case ASTNodeType::ADD:
      return generate_for_add_node(node);
    case ASTNodeType::SUB:
      return generate_for_sub_node(node);
    case ASTNodeType::NEG:
      return generate_for_neg_node(node);
    case ASTNodeType::EQUAL:
      return generate_for_equal_node(node);
    case ASTNodeType::NOT_EQUAL:
      return generate_for_not_equal_node(node);
    case ASTNodeType::LESS_THAN:
      return generate_for_less_than_node(node);
    case ASTNodeType::LESS_EQUAL:
      return generate_for_less_equal_node(node);
    case ASTNodeType::INTEGER:
      return generate_for_integer_node(node);
    case ASTNodeType::VARIABLE:
      return generate_for_variable_node(node);
    case ASTNodeType::ASSIGN:
      return generate_for_assign_node(node);
    case ASTNodeType::COMPOUND_STATEMENT:
      return generate_for_compound_statement_node(node);
    case ASTNodeType::IF_ELSE:
      return generate_for_if_else_node(node);
    default:
      std::stringstream error_msg;
      error_msg << "Unknow type of AST Node" << static_cast<uint32_t>(node->type_);
      throw std::invalid_argument(error_msg.str());
  }
}

std::vector<ThreeAddressCodePtr> IRGeneratorImpl::work() {
  generate_for_node(ast_);
  return code_builder_->finish();
}

AddressPtr IRGeneratorImpl::generate_for_mul_node(const ASTNodePtr& node) {
  auto left_address = generate_for_node(node->children_[0]); 
  auto right_address = generate_for_node(node->children_[1]);
  return code_builder_->append_multiply(std::move(left_address), std::move(right_address));
}

AddressPtr IRGeneratorImpl::generate_for_div_node(const ASTNodePtr& node) {
  auto left_address = generate_for_node(node->children_[0]); 
  auto right_address = generate_for_node(node->children_[1]);
  return code_builder_->append_divide(std::move(left_address), std::move(right_address));
}

AddressPtr IRGeneratorImpl::generate_for_add_node(const ASTNodePtr& node) {
  auto left_address = generate_for_node(node->children_[0]); 
  auto right_address = generate_for_node(node->children_[1]);
  return code_builder_->append_add(std::move(left_address), std::move(right_address));
}

AddressPtr IRGeneratorImpl::generate_for_sub_node(const ASTNodePtr& node) {
  auto left_address = generate_for_node(node->children_[0]); 
  auto right_address = generate_for_node(node->children_[1]);
  return code_builder_->append_sub(std::move(left_address), std::move(right_address));
}

AddressPtr IRGeneratorImpl::generate_for_neg_node(const ASTNodePtr& node) {
  auto child_address = generate_for_node(node->children_[0]);
  return code_builder_->append_neg(std::move(child_address));
}

AddressPtr IRGeneratorImpl::generate_for_equal_node(const ASTNodePtr& node) {
  auto left_address = generate_for_node(node->children_[0]); 
  auto right_address = generate_for_node(node->children_[1]);
  return code_builder_->append_equal(std::move(left_address), std::move(right_address));
}

AddressPtr IRGeneratorImpl::generate_for_not_equal_node(const ASTNodePtr& node) {
  auto left_address = generate_for_node(node->children_[0]); 
  auto right_address = generate_for_node(node->children_[1]);
  return code_builder_->append_not_equal(std::move(left_address), std::move(right_address));
}

AddressPtr IRGeneratorImpl::generate_for_less_than_node(const ASTNodePtr& node) {
  auto left_address = generate_for_node(node->children_[0]); 
  auto right_address = generate_for_node(node->children_[1]);
  return code_builder_->append_less_than(std::move(left_address), std::move(right_address));
}

AddressPtr IRGeneratorImpl::generate_for_less_equal_node(const ASTNodePtr& node) {
  auto left_address = generate_for_node(node->children_[0]); 
  auto right_address = generate_for_node(node->children_[1]);
  return code_builder_->append_less_equal(std::move(left_address), std::move(right_address));
}

AddressPtr IRGeneratorImpl::generate_for_integer_node(const ASTNodePtr& node) {
  return Address::constant(node->literal_);
}

AddressPtr IRGeneratorImpl::generate_for_variable_node(const ASTNodePtr& node) {
  return Address::variable(node->literal_);
}

AddressPtr IRGeneratorImpl::generate_for_assign_node(const ASTNodePtr& node) {
  auto result_address = generate_for_node(node->children_[0]); 
  auto right_address = generate_for_node(node->children_[1]);
  if (result_address->type_ != AddressType::VARIABLE) {
    throw std::invalid_argument("Expect Variable on the left of assignment");
  }
  return code_builder_->append_assign(std::move(result_address), std::move(right_address));
}

AddressPtr IRGeneratorImpl::generate_for_if_else_node(const ASTNodePtr& node) {
  auto expression_address = generate_for_node(node->children_[0]);
  auto label_if_true = code_builder_->new_label();
  auto label_if_false = code_builder_->new_label();
  auto label_follow = code_builder_->new_label();
  code_builder_->append_if_else(expression_address, label_if_true, label_if_false);
  code_builder_->append_label(label_if_true);
  generate_for_node(node->children_[1]);
  code_builder_->append_goto(label_follow);
  code_builder_->append_label(label_if_false);
  if (node->children_[2] != nullptr) {
    generate_for_node(node->children_[2]);
  }
  code_builder_->append_label(label_follow);
  return nullptr;
}

AddressPtr IRGeneratorImpl::generate_for_compound_statement_node(const ASTNodePtr& node) {
  for (const auto& child : node->children_) {
    generate_for_node(child);
  }
  return nullptr;
}

std::unique_ptr<IRGenerator> CreateIRGenerator(ASTNodePtr ast) {
  return std::make_unique<IRGeneratorImpl>(std::move(ast));
}
