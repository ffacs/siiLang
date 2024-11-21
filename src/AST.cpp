#include "AST.h"
#include <sstream>

static inline ASTNodePtr astNode(ASTNodeType type, TokenPtr token) {
  return std::make_shared<ASTNode>(type, std::move(token));
}

static ASTNodePtr astNode(ASTNodeType type, TokenPtr token, std::vector<ASTNodePtr> children) {
  return std::make_shared<ASTNode>(type, std::move(token), std::move(children));
}

ASTNodePtr ASTNode::empty() {
  return astNode(ASTNodeType::EMPTY, nullptr);
}

ASTNodePtr ASTNode::multiply(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeType::MUL, nullptr, {lhs, rhs});
}

ASTNodePtr ASTNode::divide(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeType::DIV, nullptr, {lhs, rhs});
}

ASTNodePtr ASTNode::add(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeType::ADD, nullptr, {lhs, rhs});
}

ASTNodePtr ASTNode::subtract(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeType::SUB, nullptr, {lhs, rhs});
}

ASTNodePtr ASTNode::negtive(ASTNodePtr operand) {
  return astNode(ASTNodeType::NEG, nullptr, { operand });
}

ASTNodePtr ASTNode::equal(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeType::EQUAL, nullptr, {lhs, rhs});
}

ASTNodePtr ASTNode::not_equal(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeType::NOT_EQUAL, nullptr, {lhs, rhs});
}

ASTNodePtr ASTNode::less_than(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeType::LESS_THAN, nullptr, {lhs, rhs});
}

ASTNodePtr ASTNode::less_equal(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeType::LESS_EQUAL, nullptr, {lhs, rhs});
}

ASTNodePtr ASTNode::integer(TokenPtr literal) {
  return astNode(ASTNodeType::INTEGER, std::move(literal));
}

ASTNodePtr ASTNode::variable(TokenPtr literal) {
  return astNode(ASTNodeType::VARIABLE, std::move(literal));
}
ASTNodePtr ASTNode::statements(std::vector<ASTNodePtr> children) {
  return astNode(ASTNodeType::STATEMENTS, nullptr, std::move(children));
}

std::string ASTNode::to_string() const {
  std::stringstream ss;
  switch (type_) {
    case ASTNodeType::MUL:
    case ASTNodeType::DIV: 
    case ASTNodeType::ADD: 
    case ASTNodeType::SUB: {
      ss << "(";
      ss << children_[0]->to_string();
      ss << token_->literal_;
      ss << children_[1]->to_string();
      ss << ")";
      return ss.str();
    }
    case ASTNodeType::INTEGER: {
      ss << token_->literal_;
      return ss.str();
    }
    case ASTNodeType::VARIABLE: {
      ss << token_->literal_;
      return ss.str();
    } 
    case ASTNodeType::STATEMENTS: {
      ss << "statements: {";
        for (const auto& node : children_) {
          ss << node->to_string() << ";";
        }
      ss << "}";
      return ss.str();
    } default: {
      std::stringstream error_msg;
      error_msg << "unknow type of ASTNode " << static_cast<uint32_t>(type_) << " found";
      throw std::invalid_argument(error_msg.str());
    }
  }
}

bool ASTNode::operator==(const ASTNode & other) const {
  if (type_ != other.type_) return false;
  if (children_.size() != other.children_.size()) return false;
  for (size_t i = 0; i < children_.size(); i++) {
    if (*children_[i] != *other.children_[i]) {
      return false;
    }
  }
  return true;
}
