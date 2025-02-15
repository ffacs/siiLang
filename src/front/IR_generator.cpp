#include "front/IR_generator.h"
#include "IR/code_builder.h"
#include "front/context_manager.h"
#include <set>
#include <sstream>

using namespace SiiIR;

namespace front {
class IRGeneratorImpl : public IRGenerator {
public:
  IRGeneratorImpl(ASTNodePtr ast) : ast_(std::move(ast)) {
    ctx_manager_ = CreateContextManager();
  }
  std::shared_ptr<std::vector<SiiIRCodePtr>> work() override;

protected:
  ASTNodePtr ast_;
  ContextManagerPtr ctx_manager_;
  std::set<std::string> function_definitions;

  TemporaryValuePtr generate_for_rvalue_node(const ASTNodePtr &node,
                                               CodeBuilderPtr &code_builder);

  VariableValuePtr generate_for_lvalue_node(const ASTNodePtr &node,
                                              CodeBuilderPtr &code_builder);

  void generate_for_non_value_node(const ASTNodePtr &node,
                                   CodeBuilderPtr &code_builder);

  TemporaryValuePtr generate_for_mul_node(const ASTNodePtr &node,
                                            CodeBuilderPtr &code_builder);
  TemporaryValuePtr generate_for_div_node(const ASTNodePtr &node,
                                            CodeBuilderPtr &code_builder);
  TemporaryValuePtr generate_for_add_node(const ASTNodePtr &node,
                                            CodeBuilderPtr &code_builder);
  TemporaryValuePtr generate_for_sub_node(const ASTNodePtr &node,
                                            CodeBuilderPtr &code_builder);
  TemporaryValuePtr generate_for_neg_node(const ASTNodePtr &node,
                                            CodeBuilderPtr &code_builder);
  TemporaryValuePtr
  generate_for_get_address_node(const ASTNodePtr &node,
                                CodeBuilderPtr &code_builder);
  TemporaryValuePtr generate_for_equal_node(const ASTNodePtr &node,
                                              CodeBuilderPtr &code_builder);
  TemporaryValuePtr generate_for_not_equal_node(const ASTNodePtr &node,
                                                  CodeBuilderPtr &code_builder);
  TemporaryValuePtr generate_for_less_than_node(const ASTNodePtr &node,
                                                  CodeBuilderPtr &code_builder);
  TemporaryValuePtr
  generate_for_less_equal_node(const ASTNodePtr &node,
                               CodeBuilderPtr &code_builder);
  TemporaryValuePtr generate_for_integer_node(const ASTNodePtr &node,
                                                CodeBuilderPtr &code_builder);
  VariableValuePtr generate_for_identifier_node(const ASTNodePtr &node,
                                                  CodeBuilderPtr &code_builder);
  TemporaryValuePtr generate_for_assign_node(const ASTNodePtr &node,
                                               CodeBuilderPtr &code_builder);

  void generate_for_if_else_node(const ASTNodePtr &node,
                                 CodeBuilderPtr &code_builder);
  void generate_for_for_loop_node(const ASTNodePtr &node,
                                  CodeBuilderPtr &code_builder);
  void generate_for_do_while_node(const ASTNodePtr &node,
                                  CodeBuilderPtr &code_builder);
  void generate_for_while_loop_node(const ASTNodePtr &node,
                                    CodeBuilderPtr &code_builder);
  void generate_for_declaration_statement_node(const ASTNodePtr &node,
                                               CodeBuilderPtr &code_builder);
  void generate_for_compound_statement_node(const ASTNodePtr &node,
                                            CodeBuilderPtr &code_builder);
  void generate_for_variable_declaration_node(const ASTNodePtr &node,
                                              CodeBuilderPtr &code_builder);
  FunctionValuePtr
  generate_for_function_declaration_node(const ASTNodePtr &node,
                                         CodeBuilderPtr &code_builder);
  void format_condition_value(TemporaryValuePtr& condition,
                                               CodeBuilderPtr &code_builder);
};

TemporaryValuePtr
IRGeneratorImpl::generate_for_rvalue_node(const ASTNodePtr &node,
                                          CodeBuilderPtr &code_builder) {
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
  case ASTNodeKind::GET_ADDRESS:
    return generate_for_get_address_node(node, code_builder);
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
  case ASTNodeKind::IDENTIFIER: {
    auto variable = generate_for_identifier_node(node, code_builder);
    auto temporary = ctx_manager_->function_ctx()->allocate_temporary_value(
        variable->type_);
    code_builder->append_load(variable, temporary);
    return temporary;
  }
  case ASTNodeKind::ASSIGN: {
    return generate_for_assign_node(node, code_builder);
  }
  default: {
    std::stringstream error_msg;
    // not a value node
    error_msg << "Type of AST Node" << static_cast<uint32_t>(node->kind_)
              << " is not a rvalue node";
    throw std::invalid_argument(error_msg.str());
  }
  }
}

VariableValuePtr
IRGeneratorImpl::generate_for_lvalue_node(const ASTNodePtr &node,
                                          CodeBuilderPtr &code_builder) {
  switch (node->kind_) {
  case ASTNodeKind::IDENTIFIER:
    return generate_for_identifier_node(node, code_builder);
  default: {
    std::stringstream error_msg;
    // not a value node
    error_msg << "Type of AST Node" << static_cast<uint32_t>(node->kind_)
              << " is not a lvalue node";
    throw std::invalid_argument(error_msg.str());
  }
  }
}

void IRGeneratorImpl::generate_for_non_value_node(
    const ASTNodePtr &node, CodeBuilderPtr &code_builder) {
  switch (node->kind_) {
  case ASTNodeKind::EMPTY:
    return;
  case ASTNodeKind::MUL:
    generate_for_mul_node(node, code_builder);
    return;
  case ASTNodeKind::DIV:
    generate_for_div_node(node, code_builder);
    return;
  case ASTNodeKind::ADD:
    generate_for_add_node(node, code_builder);
    return;
  case ASTNodeKind::SUB:
    generate_for_sub_node(node, code_builder);
    return;
  case ASTNodeKind::EQUAL:
    generate_for_equal_node(node, code_builder);
    return;
  case ASTNodeKind::NOT_EQUAL:
    generate_for_not_equal_node(node, code_builder);
    return;
  case ASTNodeKind::LESS_THAN:
    generate_for_less_than_node(node, code_builder);
    return;
  case ASTNodeKind::LESS_EQUAL:
    generate_for_less_equal_node(node, code_builder);
    return;
  case ASTNodeKind::NEG:
    generate_for_neg_node(node, code_builder);
    return;
  case ASTNodeKind::GET_ADDRESS:
    generate_for_get_address_node(node, code_builder);
    return;
  case ASTNodeKind::INTEGER:
    generate_for_integer_node(node, code_builder);
    return;
  case ASTNodeKind::IDENTIFIER:
    generate_for_identifier_node(node, code_builder);
    return;
  case ASTNodeKind::ASSIGN:
    generate_for_assign_node(node, code_builder);
    return;
  case ASTNodeKind::IF_ELSE:
    generate_for_if_else_node(node, code_builder);
    return;
  case ASTNodeKind::FOR_LOOP:
    generate_for_for_loop_node(node, code_builder);
    return;
  case ASTNodeKind::DO_WHILE:
    generate_for_do_while_node(node, code_builder);
    return;
  case ASTNodeKind::WHILE_LOOP:
    generate_for_while_loop_node(node, code_builder);
    return;
  case ASTNodeKind::COMPOUND_STATEMENT:
    generate_for_compound_statement_node(node, code_builder);
    return;
  case ASTNodeKind::DECLARATION_STATEMENT:
    generate_for_declaration_statement_node(node, code_builder);
    return;
  case ASTNodeKind::VARIABLE_DECLARATION:
    generate_for_variable_declaration_node(node, code_builder);
    return;
  default:
    std::stringstream error_msg;
    error_msg << "Type of AST Node" << static_cast<uint32_t>(node->kind_)
              << " is not a non-value node";
    throw std::invalid_argument(error_msg.str());
  }
}

std::shared_ptr<std::vector<SiiIRCodePtr>> IRGeneratorImpl::work() {
  CodeBuilderPtr code_builder = CreateCodeBuilder();
  if (ast_->kind_ == ASTNodeKind::FUNCTION_DECLARATION) {
    generate_for_function_declaration_node(ast_, code_builder);
  } else if (ast_->kind_ == ASTNodeKind::DECLARATION_STATEMENT) {
    generate_for_declaration_statement_node(ast_, code_builder);
  } else {
    std::stringstream error_msg;
    error_msg << "Type of AST Node" << static_cast<uint32_t>(ast_->kind_)
              << " is not a function declaration or declaration statement";
    throw std::invalid_argument(error_msg.str());
  }
  return code_builder->finish();
}

TemporaryValuePtr
IRGeneratorImpl::generate_for_mul_node(const ASTNodePtr &node,
                                       CodeBuilderPtr &code_builder) {
  const BinaryOperationNode *binary_operation_node =
      static_cast<const BinaryOperationNode *>(node.get());
  auto left_value =
      generate_for_rvalue_node(binary_operation_node->lhs_, code_builder);
  auto right_value =
      generate_for_rvalue_node(binary_operation_node->rhs_, code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_temporary_value(
      left_value->type_);
  code_builder->append_multiply(std::move(left_value),
                                std::move(right_value), result);
  return result;
}

TemporaryValuePtr
IRGeneratorImpl::generate_for_div_node(const ASTNodePtr &node,
                                       CodeBuilderPtr &code_builder) {
  const BinaryOperationNode *binary_operation_node =
      static_cast<const BinaryOperationNode *>(node.get());
  auto left_value =
      generate_for_rvalue_node(binary_operation_node->lhs_, code_builder);
  auto right_value =
      generate_for_rvalue_node(binary_operation_node->rhs_, code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_temporary_value(
      left_value->type_);
  code_builder->append_divide(std::move(left_value), std::move(right_value),
                              result);
  return result;
}

TemporaryValuePtr
IRGeneratorImpl::generate_for_add_node(const ASTNodePtr &node,
                                       CodeBuilderPtr &code_builder) {
  const BinaryOperationNode *binary_operation_node =
      static_cast<const BinaryOperationNode *>(node.get());
  auto left_value =
      generate_for_rvalue_node(binary_operation_node->lhs_, code_builder);
  auto right_value =
      generate_for_rvalue_node(binary_operation_node->rhs_, code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_temporary_value(
      left_value->type_);
  code_builder->append_add(std::move(left_value), std::move(right_value),
                           result);
  return result;
}

TemporaryValuePtr
IRGeneratorImpl::generate_for_sub_node(const ASTNodePtr &node,
                                       CodeBuilderPtr &code_builder) {
  const BinaryOperationNode *binary_operation_node =
      static_cast<const BinaryOperationNode *>(node.get());
  auto left_value =
      generate_for_rvalue_node(binary_operation_node->lhs_, code_builder);
  auto right_value =
      generate_for_rvalue_node(binary_operation_node->rhs_, code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_temporary_value(
      left_value->type_);
  code_builder->append_sub(std::move(left_value), std::move(right_value),
                           result);
  return result;
}

TemporaryValuePtr
IRGeneratorImpl::generate_for_neg_node(const ASTNodePtr &node,
                                       CodeBuilderPtr &code_builder) {
  const UnaryOperationNode *unary_operation_node =
      static_cast<const UnaryOperationNode *>(node.get());
  auto child_value =
      generate_for_rvalue_node(unary_operation_node->operand_, code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_temporary_value(
      child_value->type_);
  code_builder->append_neg(std::move(child_value), result);
  return result;
}

TemporaryValuePtr
IRGeneratorImpl::generate_for_get_address_node(const ASTNodePtr &node,
                                               CodeBuilderPtr &code_builder) {
  const UnaryOperationNode *unary_operation_node =
      static_cast<const UnaryOperationNode *>(node.get());
  auto child_value =
      generate_for_lvalue_node(unary_operation_node->operand_, code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_variable_value(child_value->type_);
  code_builder->append_alloca(result, 8);
  code_builder->append_store(std::move(child_value), result);
  auto loaded =
      ctx_manager_->function_ctx()->allocate_temporary_value(result->allocated_type_);
  code_builder->append_load(std::move(result), loaded);
  return loaded;
}
TemporaryValuePtr
IRGeneratorImpl::generate_for_equal_node(const ASTNodePtr &node,
                                         CodeBuilderPtr &code_builder) {
  const BinaryOperationNode *binary_operation_node =
      static_cast<const BinaryOperationNode *>(node.get());
  auto left_value =
      generate_for_rvalue_node(binary_operation_node->lhs_, code_builder);
  auto right_value =
      generate_for_rvalue_node(binary_operation_node->rhs_, code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_temporary_value(
      SiiIR::Type::Integer(1));
  code_builder->append_equal(std::move(left_value), std::move(right_value),
                             result);
  return result;
}

TemporaryValuePtr
IRGeneratorImpl::generate_for_not_equal_node(const ASTNodePtr &node,
                                             CodeBuilderPtr &code_builder) {
  const BinaryOperationNode *binary_operation_node =
      static_cast<const BinaryOperationNode *>(node.get());
  auto left_value =
      generate_for_rvalue_node(binary_operation_node->lhs_, code_builder);
  auto right_value =
      generate_for_rvalue_node(binary_operation_node->rhs_, code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_temporary_value(
      SiiIR::Type::Integer(1));
  code_builder->append_not_equal(std::move(left_value),
                                 std::move(right_value), result);
  return result;
}

TemporaryValuePtr
IRGeneratorImpl::generate_for_less_than_node(const ASTNodePtr &node,
                                             CodeBuilderPtr &code_builder) {
  const BinaryOperationNode *binary_operation_node =
      static_cast<const BinaryOperationNode *>(node.get());
  auto left_value =
      generate_for_rvalue_node(binary_operation_node->lhs_, code_builder);
  auto right_value =
      generate_for_rvalue_node(binary_operation_node->rhs_, code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_temporary_value(
      SiiIR::Type::Integer(1));
  code_builder->append_less_than(std::move(left_value),
                                 std::move(right_value), result);
  return result;
}

TemporaryValuePtr
IRGeneratorImpl::generate_for_less_equal_node(const ASTNodePtr &node,
                                              CodeBuilderPtr &code_builder) {
  const BinaryOperationNode *binary_operation_node =
      static_cast<const BinaryOperationNode *>(node.get());
  auto left_value =
      generate_for_rvalue_node(binary_operation_node->lhs_, code_builder);
  auto right_value =
      generate_for_rvalue_node(binary_operation_node->rhs_, code_builder);
  auto result = ctx_manager_->function_ctx()->allocate_temporary_value(
      SiiIR::Type::Integer(1));
  code_builder->append_less_equal(std::move(left_value),
                                  std::move(right_value), result);
  return result;
}

TemporaryValuePtr
IRGeneratorImpl::generate_for_integer_node(const ASTNodePtr &node,
                                           CodeBuilderPtr &code_builder) {
  const LiteralNode *literal_node =
      static_cast<const LiteralNode *>(node.get());
  return Value::constant(literal_node->literal_, SiiIR::Type::Integer(32));
}

VariableValuePtr
IRGeneratorImpl::generate_for_identifier_node(const ASTNodePtr &node,
                                              CodeBuilderPtr &code_builder) {
  const LiteralNode *literal_node =
      static_cast<const LiteralNode *>(node.get());
  SymbolContext *symbol_ctx = ctx_manager_->symbol_ctx();
  auto identifier = literal_node->literal_;
  SymbolPtr symbol;
  while (symbol_ctx != nullptr) {
    symbol = symbol_ctx->symble_table_->find(identifier);
    if (symbol != nullptr) {
      break;
    }
    symbol_ctx = symbol_ctx->father_;
  }
  if (symbol != nullptr) {
    if (symbol->value_->kind_ == ValueKind::VARIABLE) {
      return std::static_pointer_cast<VariableValue>(symbol->value_);
    } else {
      throw std::invalid_argument("Use of undeclared identifier '" +
                                  identifier + "'");
    }
  }
  throw std::invalid_argument("Use of undeclared identifier '" + identifier +
                              "'");
}

TemporaryValuePtr
IRGeneratorImpl::generate_for_assign_node(const ASTNodePtr &node,
                                          CodeBuilderPtr &code_builder) {
  const BinaryOperationNode *binary_operation_node =
      static_cast<const BinaryOperationNode *>(node.get());
  if (binary_operation_node->lhs_->kind_ != ASTNodeKind::IDENTIFIER) {
    throw std::invalid_argument("Expect Identifier on the left of assignment");
  }
  auto right_value =
      generate_for_rvalue_node(binary_operation_node->rhs_, code_builder);
  VariableValuePtr left_value =
      generate_for_lvalue_node(binary_operation_node->lhs_, code_builder);
  if (left_value->kind_ != ValueKind::VARIABLE) {
    throw std::invalid_argument("Expect Variable on the left of assignment");
  }
  code_builder->append_store(right_value, std::move(left_value));
  return right_value;
}

void IRGeneratorImpl::generate_for_if_else_node(const ASTNodePtr &node,
                                                CodeBuilderPtr &code_builder) {
  const IfElseNode *if_else_node = static_cast<const IfElseNode *>(node.get());
  auto cond = if_else_node->expression_;
  auto if_true_statement = if_else_node->if_statement_;
  auto else_statement = if_else_node->else_statement_;
  auto cond_value = generate_for_rvalue_node(cond, code_builder);
  format_condition_value(cond_value, code_builder);

  // IF
  auto [true_label_future, false_label_future] =
      code_builder->append_condition_branch(cond_value);
  auto true_label = ctx_manager_->function_ctx()->allocate_label();
  true_label_future->set_label(true_label);
  code_builder->append_label(true_label);
  generate_for_non_value_node(if_true_statement, code_builder);
  auto true_end_label_future = code_builder->append_goto();

  // Else
  auto false_label = ctx_manager_->function_ctx()->allocate_label();
  false_label_future->set_label(false_label);
  code_builder->append_label(false_label);
  if (else_statement != nullptr) {
    generate_for_non_value_node(else_statement, code_builder);
  }
  auto false_end_label_future = code_builder->append_goto();

  auto end_label = ctx_manager_->function_ctx()->allocate_label();
  true_end_label_future->set_label(end_label);
  false_end_label_future->set_label(end_label);
  code_builder->append_label(end_label);
}

void IRGeneratorImpl::generate_for_for_loop_node(const ASTNodePtr &node,
                                                 CodeBuilderPtr &code_builder) {
  const ForLoopNode *for_loop_node =
      static_cast<const ForLoopNode *>(node.get());
  auto init = for_loop_node->init_expression_;
  auto cond = for_loop_node->condition_expression_;
  auto incr = for_loop_node->increment_expression_;
  auto statement = for_loop_node->statement_;
  generate_for_non_value_node(init, code_builder);
  auto cond_label = ctx_manager_->function_ctx()->allocate_label();
  code_builder->append_label(cond_label);
  LabelFuturePtr false_label_future = nullptr;
  LabelFuturePtr true_label_future = nullptr;
  if (cond->kind_ != ASTNodeKind::EMPTY) {
    auto cond_value = generate_for_rvalue_node(cond, code_builder);
    format_condition_value(cond_value, code_builder);
    std::tie(true_label_future, false_label_future) =
        code_builder->append_condition_branch(cond_value);

    auto true_label = ctx_manager_->function_ctx()->allocate_label();
    true_label_future->set_label(true_label);
    code_builder->append_label(true_label);
  }
  generate_for_non_value_node(statement, code_builder);
  generate_for_non_value_node(incr, code_builder);
  code_builder->append_goto(cond_label);
  if (false_label_future != nullptr) {
    auto false_label = ctx_manager_->function_ctx()->allocate_label();
    false_label_future->set_label(false_label);
    code_builder->append_label(false_label);
  }
}

void IRGeneratorImpl::generate_for_while_loop_node(
    const ASTNodePtr &node, CodeBuilderPtr &code_builder) {
  const WhileLoopNode *while_loop_node =
      static_cast<const WhileLoopNode *>(node.get());
  auto cond = while_loop_node->condition_expression_;
  auto statement = while_loop_node->statement_;

  auto cond_label = ctx_manager_->function_ctx()->allocate_label();
  code_builder->append_label(cond_label);

  auto cond_value = generate_for_rvalue_node(cond, code_builder);
  format_condition_value(cond_value, code_builder);
  auto [true_label_future, false_label_future] =
      code_builder->append_condition_branch(cond_value);

  auto true_label = ctx_manager_->function_ctx()->allocate_label();
  true_label_future->set_label(true_label);
  code_builder->append_label(true_label);
  generate_for_non_value_node(statement, code_builder);
  code_builder->append_goto(cond_label);

  auto false_label = ctx_manager_->function_ctx()->allocate_label();
  false_label_future->set_label(false_label);
  code_builder->append_label(false_label);
}

void IRGeneratorImpl::generate_for_do_while_node(const ASTNodePtr &node,
                                                 CodeBuilderPtr &code_builder) {
  const DoWhileNode *do_while_node =
      static_cast<const DoWhileNode *>(node.get());
  auto statement = do_while_node->statement_;
  auto cond = do_while_node->condition_expression_;

  auto true_label = ctx_manager_->function_ctx()->allocate_label();
  code_builder->append_label(true_label);
  generate_for_non_value_node(statement, code_builder);
  auto cond_value = generate_for_rvalue_node(cond, code_builder);
  format_condition_value(cond_value, code_builder);

  auto false_label = ctx_manager_->function_ctx()->allocate_label();
  code_builder->append_condition_branch(cond_value, true_label, false_label);
  code_builder->append_label(false_label);
}

void IRGeneratorImpl::generate_for_compound_statement_node(
    const ASTNodePtr &node, CodeBuilderPtr &code_builder) {
  const CompoundStatementNode *compound_statement_node =
      static_cast<const CompoundStatementNode *>(node.get());
  ctx_manager_->push_symbol_ctx();
  for (const auto &child : compound_statement_node->children_) {
    generate_for_non_value_node(child, code_builder);
  }
  ctx_manager_->pop_symbol_ctx();
}

void IRGeneratorImpl::generate_for_declaration_statement_node(
    const ASTNodePtr &node, CodeBuilderPtr &code_builder) {
  const auto &typed_node = static_cast<const DeclarationStatementNode &>(*node);
  for (const auto &declaration : typed_node.declaration_list_) {
    generate_for_non_value_node(declaration, code_builder);
  }
}

void IRGeneratorImpl::generate_for_variable_declaration_node(
    const ASTNodePtr &node, CodeBuilderPtr &code_builder) {
  auto declarator_node = static_cast<const VariableDeclarationNode &>(*node);
  SymbolContext *ctx = ctx_manager_->symbol_ctx();
  const auto &declarator = declarator_node.declarator_;
  const auto &type = declarator->type_;
  const auto &name = declarator->identifier_;
  SiiIR::TypePtr ir_type_ptr = Type::ToIRType(type);
  VariableValuePtr identifier_value =
      ctx_manager_->function_ctx()->allocate_variable_value(ir_type_ptr);
  ctx_manager_->append_variable(
      name, Symbol::symbol(std::move(type), identifier_value));
  code_builder->append_alloca(identifier_value, Type::SizeOf(type));
  auto init_expr = declarator_node.initializer_;
  if (init_expr != nullptr) {
    auto expr_value = generate_for_rvalue_node(init_expr, code_builder);
    code_builder->append_store(expr_value, identifier_value);
  }
}

FunctionValuePtr IRGeneratorImpl::generate_for_function_declaration_node(
    const ASTNodePtr &node, CodeBuilderPtr &code_builder) {
  const auto &function_node =
      static_cast<const FunctionDeclarationNode &>(*node);
  const auto &function_name = function_node.declarator_->identifier_;
  const auto &type = function_node.declarator_->type_;
  const auto &function_type =
      static_cast<const FunctionType &>(*function_node.declarator_->type_);
  const auto &function_body = function_node.body_;
  std::shared_ptr<std::vector<SiiIRCodePtr>> function_codes;
  ctx_manager_->enter_function();
  if (function_node.body_) {
    ctx_manager_->push_symbol_ctx();
    for (auto &parameter : function_type.parameter_types_) {
      auto parameter_type = parameter->type_;
      SiiIR::TypePtr ir_type_ptr = Type::ToIRType(parameter_type);
      VariableValuePtr parameter_variable =
          ctx_manager_->function_ctx()->allocate_variable_value(ir_type_ptr);
      ctx_manager_->append_variable(
          parameter->identifier_,
          Symbol::symbol(parameter->type_, parameter_variable));
    }
    CodeBuilderPtr body_builder = CreateCodeBuilder();
    generate_for_non_value_node(function_body, body_builder);
    function_codes = body_builder->finish();
    ctx_manager_->pop_symbol_ctx();
  }
  auto function_ctx = ctx_manager_->leave_function();
  SiiIR::TypePtr ir_function_type = Type::ToIRType(type);
  FunctionValuePtr function_value = Value::Function(
      function_codes, function_ctx, function_name, std::move(ir_function_type));
  ctx_manager_->append_function(function_name,
                                Symbol::symbol(type, function_value));
  if (function_body) {
    code_builder->append_function(function_value);
  }
  return function_value;
}

void
IRGeneratorImpl::format_condition_value(TemporaryValuePtr& value,
                                          CodeBuilderPtr &code_builder) {
  if (*value->type_ == *SiiIR::Type::Integer(1)) {
    return ;
  }
  if (value->type_->kind_ == SiiIR::Type::Kind::INT) {
    auto temporary_constant = SiiIR::Value::constant("0", value->type_);
    TemporaryValuePtr result =
        ctx_manager_->function_ctx()->allocate_temporary_value(
            SiiIR::Type::Integer(1));
    code_builder->append_not_equal(value, temporary_constant, result);
    value = result;
  } else {
    throw std::runtime_error("condition type error");
  }
}

std::unique_ptr<IRGenerator> CreateIRGenerator(ASTNodePtr ast) {
  return std::make_unique<IRGeneratorImpl>(std::move(ast));
}

} // namespace front
