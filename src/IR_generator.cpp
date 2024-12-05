#include "IR_generator.h"
#include "code_builder.h"
#include "context_manager.h"
#include <set>

class IRGeneratorImpl : public IRGenerator {
 public:
  IRGeneratorImpl(ASTNodePtr ast) : ast_(std::move(ast)) {
    ctx_manager_ = CreateContextManager();
  }
  std::vector<ThreeAddressCodePtr> work() override;

protected:
  ASTNodePtr ast_;
  ContextManagerPtr ctx_manager_;
  std::set<std::string> function_definitions;

  AddressPtr generate_for_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_mul_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_div_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_add_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_sub_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_neg_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_equal_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_not_equal_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_less_than_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_less_equal_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_integer_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_identifier_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_assign_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_if_else_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_for_loop_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_do_while_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_while_loop_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_declaration_statement_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_compound_statement_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_variable_declaration_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
  AddressPtr generate_for_function_declaration_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder);
};

AddressPtr IRGeneratorImpl::generate_for_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  switch (node->kind_) {
    case ASTNodeKind::EMPTY:
      return nullptr;
    case ASTNodeKind::MUL:
      return generate_for_mul_node(node, code_builder);
    case ASTNodeKind::DIV:
      return generate_for_div_node(node, code_builder);
    case ASTNodeKind::ADD:
      return generate_for_add_node(node, code_builder);
    case ASTNodeKind::SUB:
      return generate_for_sub_node(node, code_builder);
    case ASTNodeKind::NEG:
      return generate_for_neg_node(node, code_builder);
    case ASTNodeKind::EQUAL:
      return generate_for_equal_node(node, code_builder);
    case ASTNodeKind::NOT_EQUAL:
      return generate_for_not_equal_node(node, code_builder);
    case ASTNodeKind::LESS_THAN:
      return generate_for_less_than_node(node, code_builder);
    case ASTNodeKind::LESS_EQUAL:
      return generate_for_less_equal_node(node, code_builder);
    case ASTNodeKind::INTEGER:
      return generate_for_integer_node(node, code_builder);
    case ASTNodeKind::IDENTIFIER:
      return generate_for_identifier_node(node, code_builder);
    case ASTNodeKind::ASSIGN:
      return generate_for_assign_node(node, code_builder);
    case ASTNodeKind::COMPOUND_STATEMENT:
      return generate_for_compound_statement_node(node, code_builder);
    case ASTNodeKind::IF_ELSE:
      return generate_for_if_else_node(node, code_builder);
    case ASTNodeKind::FOR_LOOP:
      return generate_for_for_loop_node(node, code_builder);
    case ASTNodeKind::DO_WHILE:
      return generate_for_do_while_node(node, code_builder);
    case ASTNodeKind::WHILE_LOOP:
      return generate_for_while_loop_node(node, code_builder);
    case ASTNodeKind::DECLARATION:
      return generate_for_declaration_statement_node(node, code_builder);
    case ASTNodeKind::VARIABLE_DECLARATION:
      return generate_for_variable_declaration_node(node, code_builder);
    case ASTNodeKind::FUNCTION_DECLARATION:
      return generate_for_function_declaration_node(node, code_builder);
    default:
      std::stringstream error_msg;
      error_msg << "Unknow type of AST Node" << static_cast<uint32_t>(node->kind_);
      throw std::invalid_argument(error_msg.str());
  }
}

std::vector<ThreeAddressCodePtr> IRGeneratorImpl::work() {
  CodeBuilderPtr code_builder = CreateCodeBuilder();
  generate_for_node(ast_, code_builder);
  return code_builder->finish();
}

AddressPtr IRGeneratorImpl::generate_for_mul_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  auto left_address = generate_for_node(node->children_[0], code_builder); 
  auto right_address = generate_for_node(node->children_[1], code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_temporary_address();
  code_builder->append_multiply(std::move(left_address), std::move(right_address), result);
  return result;
}

AddressPtr IRGeneratorImpl::generate_for_div_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  auto left_address = generate_for_node(node->children_[0], code_builder); 
  auto right_address = generate_for_node(node->children_[1], code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_temporary_address();
  code_builder->append_divide(std::move(left_address), std::move(right_address), result);
  return result;
}

AddressPtr IRGeneratorImpl::generate_for_add_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  auto left_address = generate_for_node(node->children_[0], code_builder); 
  auto right_address = generate_for_node(node->children_[1], code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_temporary_address();
  code_builder->append_add(std::move(left_address), std::move(right_address), result);
  return result;
}

AddressPtr IRGeneratorImpl::generate_for_sub_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  auto left_address = generate_for_node(node->children_[0], code_builder); 
  auto right_address = generate_for_node(node->children_[1], code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_temporary_address();
  code_builder->append_sub(std::move(left_address), std::move(right_address), result);
  return result;
}

AddressPtr IRGeneratorImpl::generate_for_neg_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  auto child_address = generate_for_node(node->children_[0], code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_temporary_address();
  code_builder->append_neg(std::move(child_address), result);
  return result;
}

AddressPtr IRGeneratorImpl::generate_for_equal_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  auto left_address = generate_for_node(node->children_[0], code_builder); 
  auto right_address = generate_for_node(node->children_[1], code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_temporary_address();
  code_builder->append_equal(std::move(left_address), std::move(right_address), result);
  return result;
}

AddressPtr IRGeneratorImpl::generate_for_not_equal_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  auto left_address = generate_for_node(node->children_[0], code_builder); 
  auto right_address = generate_for_node(node->children_[1], code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_temporary_address();
  code_builder->append_not_equal(std::move(left_address), std::move(right_address), result);
  return result;
}

AddressPtr IRGeneratorImpl::generate_for_less_than_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  auto left_address = generate_for_node(node->children_[0], code_builder); 
  auto right_address = generate_for_node(node->children_[1], code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_temporary_address();
  code_builder->append_less_than(std::move(left_address), std::move(right_address), result);
  return result;
}

AddressPtr IRGeneratorImpl::generate_for_less_equal_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  auto left_address = generate_for_node(node->children_[0], code_builder); 
  auto right_address = generate_for_node(node->children_[1], code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_temporary_address();
  code_builder->append_less_equal(std::move(left_address), std::move(right_address), result);
  return result;
}

AddressPtr IRGeneratorImpl::generate_for_integer_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  return Address::constant(node->literal_);
}

AddressPtr IRGeneratorImpl::generate_for_identifier_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  SymbolContext* symbol_ctx = ctx_manager_->symbol_ctx();
  auto identifier = node->literal_;
  SymbolPtr symbol;
  while (symbol_ctx != nullptr) {
    symbol = symbol_ctx->symble_table_->find(identifier);
    if (symbol != nullptr) { break; }
    symbol_ctx = symbol_ctx->father_;
  }
  if (symbol != nullptr) {
    return symbol->address_;
  }
  throw std::invalid_argument("Use of undeclared identifier '" + identifier + "'");

}

AddressPtr IRGeneratorImpl::generate_for_assign_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  auto result_address = generate_for_node(node->children_[0], code_builder); 
  auto right_address = generate_for_node(node->children_[1], code_builder);
  if (result_address->type_ != AddressType::VARIABLE) {
    throw std::invalid_argument("Expect Variable on the left of assignment");
  }
  return code_builder->append_assign(std::move(result_address), std::move(right_address));
}

AddressPtr IRGeneratorImpl::generate_for_if_else_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  auto cond = node->children_[0];
  auto if_true_statement = node->children_[1];
  auto else_statement = node->children_[2];
  auto cond_address = generate_for_node(node->children_[0], code_builder);
  auto false_label = code_builder->new_label();
  auto end_label = code_builder->new_label();

  // IF
  code_builder->append_if_false_goto(cond_address, false_label);
  generate_for_node(if_true_statement, code_builder);
  code_builder->append_goto(end_label);

  // Else
  code_builder->append_label(false_label);
  if (else_statement != nullptr) {
    generate_for_node(else_statement, code_builder);
  }

  code_builder->append_label(end_label);
  return nullptr;
}

AddressPtr IRGeneratorImpl::generate_for_for_loop_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  auto init = node->children_[0];
  auto cond = node->children_[1];
  auto incr = node->children_[2];
  auto statement = node->children_[3];
  generate_for_node(init, code_builder);
  auto cond_label = code_builder->new_label();
  auto end_label  = code_builder->new_label();
  code_builder->append_label(cond_label);
  if (cond->kind_ != ASTNodeKind::EMPTY) {
    auto cond_address = generate_for_node(cond, code_builder);
    code_builder->append_if_false_goto(cond_address, end_label);
  }
  generate_for_node(statement, code_builder);
  generate_for_node(incr, code_builder);
  code_builder->append_goto(cond_label);
  code_builder->append_label(end_label);
  return nullptr;
}

AddressPtr IRGeneratorImpl::generate_for_while_loop_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  auto cond = node->children_[0];
  auto statement = node->children_[1];

  auto cond_label = code_builder->new_label();
  auto end_label  = code_builder->new_label();

  code_builder->append_label(cond_label);
  auto cond_address = generate_for_node(cond, code_builder);
  code_builder->append_if_false_goto(cond_address, end_label);
  generate_for_node(statement, code_builder);
  code_builder->append_goto(cond_label);
  code_builder->append_label(end_label);
  return nullptr;
}

AddressPtr IRGeneratorImpl::generate_for_do_while_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  auto statement = node->children_[0];
  auto cond = node->children_[1];

  auto statement_label = code_builder->new_label();

  code_builder->append_label(statement_label);
  generate_for_node(statement, code_builder);
  auto cond_address = generate_for_node(cond, code_builder);
  code_builder->append_if_true_goto(cond_address, statement_label);
  return nullptr;
}

AddressPtr IRGeneratorImpl::generate_for_compound_statement_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  ctx_manager_->push_symbol_ctx();
  for (const auto& child : node->children_) {
    generate_for_node(child, code_builder);
  }
  ctx_manager_->pop_symbol_ctx();
  return nullptr;
}

AddressPtr IRGeneratorImpl::generate_for_declaration_statement_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  const auto& typed_node = static_cast<const DeclarationStatementNode&>(*node);
  for (const auto& declaration : typed_node.declaration_list_) {
    generate_for_node(declaration, code_builder);
  }
  return nullptr;
}

AddressPtr IRGeneratorImpl::generate_for_variable_declaration_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
   auto declarator_node = static_cast<const VariableDeclarationNode&>(*node);
   SymbolContext* ctx = ctx_manager_->symbol_ctx();
   const auto& declarator = declarator_node.declarator_;
   const auto& type = declarator->type_;
   const auto& name = declarator->identifier_;
   VariableAddressPtr identifier_address = ctx_manager_->function_ctx()->allocate_variable_address();
   ctx_manager_->append_variable(name, Symbol::symbol(std::move(type), identifier_address));
   code_builder->append_alloca(identifier_address, Type::SizeOf(type));
   auto init_expr = declarator_node.initializer_;
   if (init_expr != nullptr) {
     auto expr_address = generate_for_node(init_expr, code_builder);
     code_builder->append_assign(identifier_address, expr_address);
   }
   return nullptr;
}

AddressPtr IRGeneratorImpl::generate_for_function_declaration_node(const ASTNodePtr& node, CodeBuilderPtr& code_builder) {
  const auto& function_node = static_cast<const FunctionDeclarationNode&>(*node);
  const auto& function_name = function_node.declarator_->identifier_;
  const auto& type = function_node.declarator_->type_;
  const auto& function_type = static_cast<const FunctionType&>(*function_node.declarator_->type_);
  const auto& function_body = function_node.body_;
  std::shared_ptr<std::vector<ThreeAddressCodePtr>> function_codes;
  ctx_manager_->enter_function();
  if (function_node.body_) {
    ctx_manager_->push_symbol_ctx();
    for (auto& parameter : function_type.parameter_types_) {
      VariableAddressPtr parameter_variable = ctx_manager_->function_ctx()->allocate_variable_address();
      ctx_manager_->append_variable(parameter->identifier_, Symbol::symbol(type, parameter_variable));
    }
    CodeBuilderPtr body_builder = CreateCodeBuilder();
    generate_for_node(function_body, body_builder);
    function_codes = std::make_shared<std::vector<ThreeAddressCodePtr>>(body_builder->finish());
    ctx_manager_->pop_symbol_ctx();
  }
  ctx_manager_->leave_function();
  FunctionAddressPtr function_address = Address::function(function_codes, function_name);
  ctx_manager_->append_function(function_name, Symbol::symbol(type, function_address));
  if (function_body) {
    code_builder->append_function(function_address);
  }
  return function_address;
}

std::unique_ptr<IRGenerator> CreateIRGenerator(ASTNodePtr ast) {
  return std::make_unique<IRGeneratorImpl>(std::move(ast));
}
