#include "IR_generator.h"
#include "code_builder.h"
#include "context_manager.h"

class IRGeneratorImpl : public IRGenerator {
 public:
  IRGeneratorImpl(ASTNodePtr ast) : ast_(std::move(ast)) {
    code_builder_ = CreateCodeBuilder();
    ctx_manager_ = CreateContextManager();
  }
  std::vector<ThreeAddressCodePtr> work() override;

protected:
  ASTNodePtr ast_;
  std::unique_ptr<CodeBuilder> code_builder_;
  ContextManagerPtr ctx_manager_;

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
  AddressPtr generate_for_for_loop_node(const ASTNodePtr& node);
  AddressPtr generate_for_do_while_node(const ASTNodePtr& node);
  AddressPtr generate_for_while_loop_node(const ASTNodePtr& node);
  AddressPtr generate_for_declaration_node(const ASTNodePtr& node);
  AddressPtr generate_for_single_declaration_node(const ASTNodePtr& node);
  AddressPtr generate_for_compound_statement_node(const ASTNodePtr& node);
};

AddressPtr IRGeneratorImpl::generate_for_node(const ASTNodePtr& node) {
  switch (node->kind_) {
    case ASTNodeKind::EMPTY:
      return nullptr;
    case ASTNodeKind::MUL:
      return generate_for_mul_node(node);
    case ASTNodeKind::DIV:
      return generate_for_div_node(node);
    case ASTNodeKind::ADD:
      return generate_for_add_node(node);
    case ASTNodeKind::SUB:
      return generate_for_sub_node(node);
    case ASTNodeKind::NEG:
      return generate_for_neg_node(node);
    case ASTNodeKind::EQUAL:
      return generate_for_equal_node(node);
    case ASTNodeKind::NOT_EQUAL:
      return generate_for_not_equal_node(node);
    case ASTNodeKind::LESS_THAN:
      return generate_for_less_than_node(node);
    case ASTNodeKind::LESS_EQUAL:
      return generate_for_less_equal_node(node);
    case ASTNodeKind::INTEGER:
      return generate_for_integer_node(node);
    case ASTNodeKind::VARIABLE:
      return generate_for_variable_node(node);
    case ASTNodeKind::ASSIGN:
      return generate_for_assign_node(node);
    case ASTNodeKind::COMPOUND_STATEMENT:
      return generate_for_compound_statement_node(node);
    case ASTNodeKind::IF_ELSE:
      return generate_for_if_else_node(node);
    case ASTNodeKind::FOR_LOOP:
      return generate_for_for_loop_node(node);
    case ASTNodeKind::DO_WHILE:
      return generate_for_do_while_node(node);
    case ASTNodeKind::WHILE_LOOP:
      return generate_for_while_loop_node(node);
    case ASTNodeKind::DECLARATION:
      return generate_for_declaration_node(node);
    case ASTNodeKind::SINGLE_DECLARATION:
      return generate_for_single_declaration_node(node);
    default:
      std::stringstream error_msg;
      error_msg << "Unknow type of AST Node" << static_cast<uint32_t>(node->kind_);
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
  auto cond = node->children_[0];
  auto if_true_statement = node->children_[1];
  auto else_statement = node->children_[2];
  auto cond_address = generate_for_node(node->children_[0]);
  auto false_label = code_builder_->new_label();
  auto end_label = code_builder_->new_label();

  // IF
  code_builder_->append_if_false_goto(cond_address, false_label);
  generate_for_node(if_true_statement);
  code_builder_->append_goto(end_label);

  // Else
  code_builder_->append_label(false_label);
  if (else_statement != nullptr) {
    generate_for_node(else_statement);
  }

  code_builder_->append_label(end_label);
  return nullptr;
}

AddressPtr IRGeneratorImpl::generate_for_for_loop_node(const ASTNodePtr& node) {
  auto init = node->children_[0];
  auto cond = node->children_[1];
  auto incr = node->children_[2];
  auto statement = node->children_[3];
  generate_for_node(init);
  auto cond_label = code_builder_->new_label();
  auto end_label  = code_builder_->new_label();
  code_builder_->append_label(cond_label);
  if (cond->kind_ != ASTNodeKind::EMPTY) {
    auto cond_address = generate_for_node(cond);
    code_builder_->append_if_false_goto(cond_address, end_label);
  }
  generate_for_node(statement);
  generate_for_node(incr);
  code_builder_->append_goto(cond_label);
  code_builder_->append_label(end_label);
  return nullptr;
}

AddressPtr IRGeneratorImpl::generate_for_while_loop_node(const ASTNodePtr& node) {
  auto cond = node->children_[0];
  auto statement = node->children_[1];

  auto cond_label = code_builder_->new_label();
  auto end_label  = code_builder_->new_label();

  code_builder_->append_label(cond_label);
  auto cond_address = generate_for_node(cond);
  code_builder_->append_if_false_goto(cond_address, end_label);
  generate_for_node(statement);
  code_builder_->append_goto(cond_label);
  code_builder_->append_label(end_label);
  return nullptr;
}

AddressPtr IRGeneratorImpl::generate_for_do_while_node(const ASTNodePtr& node) {
  auto statement = node->children_[0];
  auto cond = node->children_[1];

  auto statement_label = code_builder_->new_label();

  code_builder_->append_label(statement_label);
  generate_for_node(statement);
  auto cond_address = generate_for_node(cond);
  code_builder_->append_if_true_goto(cond_address, statement_label);
  return nullptr;
}

AddressPtr IRGeneratorImpl::generate_for_compound_statement_node(const ASTNodePtr& node) {
  for (const auto& child : node->children_) {
    generate_for_node(child);
  }
  return nullptr;
}

AddressPtr IRGeneratorImpl::generate_for_declaration_node(const ASTNodePtr& node) {
  for (const auto& child : node->children_) {
    generate_for_node(child);
  }
  return nullptr;
}

AddressPtr IRGeneratorImpl::generate_for_single_declaration_node(const ASTNodePtr& node) {
  auto decl_node = reinterpret_cast<const SingleDeclarationNode&>(*node);
  auto ctx = ctx_manager_->current_context();
  auto type = decl_node.type_;
  auto name = decl_node.identifier_->literal_;
  if (!ctx->symble_table_->push(Symbol::symbol(std::move(type), name))) {
    throw std::invalid_argument("Redefination of " + name);
  }
  auto init_expr = decl_node.initializer_;
  if (init_expr != nullptr) {
    auto identifier_address = generate_for_node(decl_node.identifier_);
    auto expr_address = generate_for_node(init_expr);
    code_builder_->append_assign(identifier_address, expr_address);
  }
  return nullptr;
}

std::unique_ptr<IRGenerator> CreateIRGenerator(ASTNodePtr ast) {
  return std::make_unique<IRGeneratorImpl>(std::move(ast));
}
