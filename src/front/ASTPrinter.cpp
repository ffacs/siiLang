#include "front/ASTPrinter.h"
#include <map>
#include <sstream>

namespace front {

  static std::map< ASTNodeKind, std::string > BINARY_OPERATION_TYPE_TO_STRING
      = {
          { ASTNodeKind::MUL,        "*"  },
          { ASTNodeKind::DIV,        "/"  },
          { ASTNodeKind::ADD,        "+"  },
          { ASTNodeKind::SUB,        "-"  },
          { ASTNodeKind::EQUAL,      "==" },
          { ASTNodeKind::NOT_EQUAL,  "!=" },
          { ASTNodeKind::LESS_THAN,  "<"  },
          { ASTNodeKind::LESS_EQUAL, "<=" },
  };

  void ASTPrintVisitor::visit( const EmptyNode& node ) {
    os_ << indent_ << "EmptyNode: "
        << "\n";
  }

  void ASTPrintVisitor::visit( const BinaryOperationNode& node ) {
    switch ( node.kind_ ) {
    case ASTNodeKind::MUL:
    case ASTNodeKind::DIV:
    case ASTNodeKind::ADD:
    case ASTNodeKind::SUB:
    case ASTNodeKind::EQUAL:
    case ASTNodeKind::NOT_EQUAL:
    case ASTNodeKind::LESS_THAN:
    case ASTNodeKind::LESS_EQUAL:
    case ASTNodeKind::ASSIGN    : {
      os_ << indent_ << "BinaryOperationNode: "
          << BINARY_OPERATION_TYPE_TO_STRING[ node.kind_ ] << "\n";
      ++indent_;
      node.lhs_->accept( *this );
      node.rhs_->accept( *this );
      --indent_;
      return;
    }
    default: {
      std::stringstream error_msg;
      error_msg << "Unknow type of BinaryOperationNode"
                << static_cast< uint32_t >( node.kind_ );
      throw std::invalid_argument( error_msg.str() );
    }
    }
  }

  void ASTPrintVisitor::visit( const UnaryOperationNode& node ) {
    switch ( node.kind_ ) {
    case ASTNodeKind::NEG: {
      os_ << indent_ << "UnaryOperationNode: -"
          << "\n";
      ++indent_;
      node.operand_->accept( *this );
      --indent_;
      return;
    }
    case ASTNodeKind::GET_ADDRESS: {
      os_ << indent_ << "UnaryOperationNode: &"
          << "\n";
      ++indent_;
      node.operand_->accept( *this );
      --indent_;
      return;
    }
    default: {
      std::stringstream error_msg;
      error_msg << "Unknow type of UnaryOperationNode"
                << static_cast< uint32_t >( node.kind_ );
      throw std::invalid_argument( error_msg.str() );
    }
    }
  }

  void ASTPrintVisitor::visit( const LiteralNode& node ) {
    switch ( node.kind_ ) {
    case ASTNodeKind::INTEGER: {
      os_ << indent_ << "LiteralNode: "
          << " int " << node.literal_ << "\n";
      return;
    }
    case ASTNodeKind::IDENTIFIER: {
      os_ << indent_ << "LiteralNode: "
          << " identifier " << node.literal_ << "\n";
      return;
    }
    default: {
      std::stringstream error_msg;
      error_msg << "Unknow type of LiteralNode"
                << static_cast< uint32_t >( node.kind_ );
      throw std::invalid_argument( error_msg.str() );
    }
    }
  }

  void ASTPrintVisitor::visit( const IfElseNode& node ) {
    os_ << indent_ << "IfElseNode: "
        << "\n";
    ++indent_;
    node.expression_->accept( *this );
    node.if_statement_->accept( *this );
    if ( node.else_statement_ ) {
      node.else_statement_->accept( *this );
    }
    --indent_;
  }

  void ASTPrintVisitor::visit( const ForLoopNode& node ) {
    os_ << indent_ << "ForLoopNode: "
        << "\n";
    ++indent_;
    node.init_expression_->accept( *this );
    node.condition_expression_->accept( *this );
    node.increment_expression_->accept( *this );
    node.statement_->accept( *this );
    --indent_;
  }

  void ASTPrintVisitor::visit( const DoWhileNode& node ) {
    os_ << indent_ << "DoWhileNode: "
        << "\n";
    ++indent_;
    node.statement_->accept( *this );
    node.condition_expression_->accept( *this );
    --indent_;
  }

  void ASTPrintVisitor::visit( const WhileLoopNode& node ) {
    os_ << indent_ << "WhileLoopNode: "
        << "\n";
    ++indent_;
    node.condition_expression_->accept( *this );
    node.statement_->accept( *this );
    --indent_;
  }

  void ASTPrintVisitor::visit( const CompoundStatementNode& node ) {
    os_ << indent_ << "CompoundStatementNode: "
        << "\n";
    ++indent_;
    for ( const auto& child : node.children_ ) {
      child->accept( *this );
    }
    --indent_;
  }

  void ASTPrintVisitor::visit( const VariableDeclarationNode& node ) {
    os_ << indent_ << "VariableDeclarationNode: "
        << "\n";
    ++indent_;
    os_ << indent_ << "Declarator: " << node.declarator_->to_string() << "\n";
    if ( node.initializer_ ) {
      node.initializer_->accept( *this );
    }
    --indent_;
  }

  void ASTPrintVisitor::visit( const FunctionDeclarationNode& node ) {
    os_ << indent_ << "FunctionDeclarationNode: "
        << "\n";
    ++indent_;
    os_ << indent_ << "Declarator: " << node.declarator_->to_string() << "\n";
    for ( const auto& child : node.declaration_statement_list_ ) {
      visit( *child );
    }
    if ( node.body_ ) {
      visit( *node.body_ );
    }
    --indent_;
  }

  void ASTPrintVisitor::visit( const DeclarationStatementNode& node ) {
    os_ << indent_ << "DeclarationStatementNode: "
        << "\n";
    ++indent_;
    for ( const auto& child : node.declaration_list_ ) {
      child->accept( *this );
    }
    --indent_;
  }

  void ASTPrintVisitor::visit( const ReturnNode& node ) {
    os_ << indent_ << "ReturnNode: "
        << "\n";
    ++indent_;
    if ( node.result_ ) {
      node.result_->accept( *this );
    }
    --indent_;
  }

}  // namespace front
