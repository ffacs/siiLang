#pragma once
#include "front/diagnose.h"
#include "front/type.h"
#include <memory>
#include <string>
#include <vector>

namespace front {

  enum class ASTNodeKind : uint32_t {
    EMPTY                 = 0,
    MUL                   = 1,
    DIV                   = 2,
    ADD                   = 3,
    SUB                   = 4,
    EQUAL                 = 5,
    NOT_EQUAL             = 6,
    LESS_THAN             = 7,
    LESS_EQUAL            = 8,
    NEG                   = 9,
    INTEGER               = 10,
    IDENTIFIER            = 11,
    ASSIGN                = 12,
    IF_ELSE               = 13,
    FOR_LOOP              = 14,
    DO_WHILE              = 15,
    WHILE_LOOP            = 16,
    COMPOUND_STATEMENT    = 17,
    VARIABLE_DECLARATION  = 18,
    FUNCTION_DECLARATION  = 19,
    DECLARATION_STATEMENT = 20,
    GET_ADDRESS           = 21,
    RETURN                = 22
  };

  class ASTNode;
  class EmptyNode;
  class BinaryOperationNode;
  class UnaryOperationNode;
  class LiteralNode;
  class IfElseNode;
  class ForLoopNode;
  class DoWhileNode;
  class WhileLoopNode;
  class CompoundStatementNode;
  class DeclarationNode;
  class VariableDeclarationNode;
  class FunctionDeclarationNode;
  class DeclarationStatementNode;
  class GetAddressNode;
  class ReturnNode;
  class ASTVisitor;
  typedef std::shared_ptr< ASTNode >                 ASTNodePtr;
  typedef std::shared_ptr< EmptyNode >               EmptyNodePtr;
  typedef std::shared_ptr< BinaryOperationNode >     BinaryOperationNodePtr;
  typedef std::shared_ptr< UnaryOperationNode >      UnaryOperationNodePtr;
  typedef std::shared_ptr< LiteralNode >             LiteralNodePtr;
  typedef std::shared_ptr< IfElseNode >              IfElseNodePtr;
  typedef std::shared_ptr< ForLoopNode >             ForLoopNodePtr;
  typedef std::shared_ptr< DoWhileNode >             DoWhileNodePtr;
  typedef std::shared_ptr< WhileLoopNode >           WhileLoopNodePtr;
  typedef std::shared_ptr< CompoundStatementNode >   CompoundStatementNodePtr;
  typedef std::shared_ptr< DeclarationNode >         DeclarationNodePtr;
  typedef std::shared_ptr< VariableDeclarationNode > VariableDeclarationNodePtr;
  typedef std::shared_ptr< FunctionDeclarationNode > FunctionDeclarationNodePtr;
  typedef std::shared_ptr< DeclarationStatementNode >
                                            DeclarationStatementNodePtr;
  typedef std::shared_ptr< GetAddressNode > GetAddressNodePtr;
  typedef std::shared_ptr< ReturnNode >     ReturnNodePtr;

  struct ASTNode {
  protected:
    ASTNode( ASTNodeKind kind )
        : kind_( kind ) {}

  public:
    ASTNodeKind kind_;
    LexInfo     lex_info_;

    virtual bool operator==( const ASTNode& ) const;
    bool         operator!=( const ASTNode& other ) const {
      return !( *this == other );
    }
    virtual void accept( ASTVisitor& visitor ) = 0;

    static ASTNodePtr             empty();
    static BinaryOperationNodePtr Multiply( ASTNodePtr lhs, ASTNodePtr rhs );
    static BinaryOperationNodePtr Divide( ASTNodePtr lhs, ASTNodePtr rhs );
    static BinaryOperationNodePtr Add( ASTNodePtr lhs, ASTNodePtr rhs );
    static BinaryOperationNodePtr Subtract( ASTNodePtr lhs, ASTNodePtr rhs );
    static BinaryOperationNodePtr Equal( ASTNodePtr lhs, ASTNodePtr rhs );
    static BinaryOperationNodePtr Not_equal( ASTNodePtr lhs, ASTNodePtr rhs );
    static BinaryOperationNodePtr Less_than( ASTNodePtr lhs, ASTNodePtr rhs );
    static BinaryOperationNodePtr Less_equal( ASTNodePtr lhs, ASTNodePtr rhs );
    static BinaryOperationNodePtr Assign( ASTNodePtr lhs, ASTNodePtr rhs );
    static LiteralNodePtr         Identifier( const std::string& name );
    static LiteralNodePtr         Integer( const std::string& literal );
    static UnaryOperationNodePtr  Negtive( ASTNodePtr operand );
    static IfElseNodePtr          If_else( ASTNodePtr expression,
                                           ASTNodePtr if_statement,
                                           ASTNodePtr else_statement );
    static ForLoopNodePtr         For_loop( ASTNodePtr init_expression,
                                            ASTNodePtr condition_expression,
                                            ASTNodePtr add_expression,
                                            ASTNodePtr statement );
    static DoWhileNodePtr         Do_while( ASTNodePtr statement,
                                            ASTNodePtr condition_expression );
    static WhileLoopNodePtr       While_loop( ASTNodePtr condition_expression,
                                              ASTNodePtr statement );
    static CompoundStatementNodePtr
    Compound_statement( std::vector< ASTNodePtr > children );
    static VariableDeclarationNodePtr
    Variable_declaration( DeclaratorPtr declarator, ASTNodePtr initializer );
    static FunctionDeclarationNodePtr
    Function_declaration( DeclaratorPtr            declarator,
                          CompoundStatementNodePtr body );
    static FunctionDeclarationNodePtr Function_declaration(
        DeclaratorPtr                              declarator,
        std::vector< DeclarationStatementNodePtr > declaration_statement_list,
        CompoundStatementNodePtr                   body );
    static DeclarationNodePtr Declaration( DeclaratorPtr declarator,
                                           ASTNodePtr    initializer );
    static DeclarationStatementNodePtr
    Declaration_statement( std::vector< DeclarationNodePtr > declaration_list );
    static VariableDeclarationNodePtr
    Normalize_variable_declaration( ASTNodePtr node );
    static FunctionDeclarationNodePtr
                              Normalize_function_declaration( ASTNodePtr node );
    static DeclarationNodePtr Normalize_declaration( ASTNodePtr node );
    static GetAddressNodePtr  Get_address( ASTNodePtr node );
    static ReturnNodePtr      Return( ASTNodePtr operand );
  };

  class ASTVisitor {
  public:
    virtual void visit( const EmptyNode& node )                = 0;
    virtual void visit( const BinaryOperationNode& node )      = 0;
    virtual void visit( const UnaryOperationNode& node )       = 0;
    virtual void visit( const LiteralNode& node )              = 0;
    virtual void visit( const IfElseNode& node )               = 0;
    virtual void visit( const ForLoopNode& node )              = 0;
    virtual void visit( const DoWhileNode& node )              = 0;
    virtual void visit( const WhileLoopNode& node )            = 0;
    virtual void visit( const CompoundStatementNode& node )    = 0;
    virtual void visit( const VariableDeclarationNode& node )  = 0;
    virtual void visit( const FunctionDeclarationNode& node )  = 0;
    virtual void visit( const DeclarationStatementNode& node ) = 0;
    virtual void visit( const ReturnNode& node )               = 0;
  };

  struct EmptyNode : public ASTNode {
    EmptyNode()
        : ASTNode( ASTNodeKind::EMPTY ) {}

    bool operator==( const ASTNode& ) const override;
    void accept( ASTVisitor& visitor ) override { visitor.visit( *this ); }
  };

  struct BinaryOperationNode : public ASTNode {
    ASTNodePtr lhs_;
    ASTNodePtr rhs_;
    BinaryOperationNode( ASTNodePtr lhs, ASTNodePtr rhs, ASTNodeKind kind )
        : ASTNode( kind )
        , lhs_( std::move( lhs ) )
        , rhs_( std::move( rhs ) ) {}

    bool operator==( const ASTNode& ) const override;
    void accept( ASTVisitor& visitor ) override { visitor.visit( *this ); }
  };

  struct UnaryOperationNode : public ASTNode {
    ASTNodePtr operand_;
    UnaryOperationNode( ASTNodePtr operand, ASTNodeKind kind )
        : ASTNode( kind )
        , operand_( std::move( operand ) ) {}

    bool operator==( const ASTNode& ) const override;
    void accept( ASTVisitor& visitor ) override { visitor.visit( *this ); }
  };

  struct LiteralNode : public ASTNode {
    std::string literal_;
    LiteralNode( std::string literal, ASTNodeKind kind )
        : ASTNode( kind )
        , literal_( std::move( literal ) ) {}

    bool operator==( const ASTNode& ) const override;
    void accept( ASTVisitor& visitor ) override { visitor.visit( *this ); }
  };

  struct IfElseNode : public ASTNode {
    ASTNodePtr expression_;
    ASTNodePtr if_statement_;
    ASTNodePtr else_statement_;
    IfElseNode( ASTNodePtr expression,
                ASTNodePtr if_statement,
                ASTNodePtr else_statement )
        : ASTNode( ASTNodeKind::IF_ELSE )
        , expression_( std::move( expression ) )
        , if_statement_( std::move( if_statement ) )
        , else_statement_( std::move( else_statement ) ) {}

    bool operator==( const ASTNode& ) const override;
    void accept( ASTVisitor& visitor ) override { visitor.visit( *this ); }
  };

  struct ForLoopNode : public ASTNode {
    ASTNodePtr init_expression_;
    ASTNodePtr condition_expression_;
    ASTNodePtr increment_expression_;
    ASTNodePtr statement_;
    ForLoopNode( ASTNodePtr init_expression,
                 ASTNodePtr condition_expression,
                 ASTNodePtr increment_expression,
                 ASTNodePtr statement )
        : ASTNode( ASTNodeKind::FOR_LOOP )
        , init_expression_( std::move( init_expression ) )
        , condition_expression_( std::move( condition_expression ) )
        , increment_expression_( std::move( increment_expression ) )
        , statement_( std::move( statement ) ) {}

    bool operator==( const ASTNode& ) const override;
    void accept( ASTVisitor& visitor ) override { visitor.visit( *this ); }
  };

  struct DoWhileNode : public ASTNode {
    ASTNodePtr statement_;
    ASTNodePtr condition_expression_;
    DoWhileNode( ASTNodePtr statement, ASTNodePtr condition_expression )
        : ASTNode( ASTNodeKind::DO_WHILE )
        , statement_( std::move( statement ) )
        , condition_expression_( std::move( condition_expression ) ) {}

    bool operator==( const ASTNode& ) const override;
    void accept( ASTVisitor& visitor ) override { visitor.visit( *this ); }
  };

  struct WhileLoopNode : public ASTNode {
    ASTNodePtr condition_expression_;
    ASTNodePtr statement_;
    WhileLoopNode( ASTNodePtr condition_expression, ASTNodePtr statement )
        : ASTNode( ASTNodeKind::WHILE_LOOP )
        , condition_expression_( std::move( condition_expression ) )
        , statement_( std::move( statement ) ) {}

    bool operator==( const ASTNode& ) const override;
    void accept( ASTVisitor& visitor ) override { visitor.visit( *this ); }
  };

  struct CompoundStatementNode : public ASTNode {
    std::vector< ASTNodePtr > children_;
    CompoundStatementNode( std::vector< ASTNodePtr > children )
        : ASTNode( ASTNodeKind::COMPOUND_STATEMENT )
        , children_( std::move( children ) ) {}

    bool operator==( const ASTNode& ) const override;
    void accept( ASTVisitor& visitor ) override { visitor.visit( *this ); }
  };

  struct DeclarationNode : public ASTNode {
  protected:
    DeclarationNode( ASTNodeKind kind, DeclaratorPtr declarator )
        : ASTNode( kind )
        , declarator_( std::move( declarator ) ) {}

  public:
    DeclaratorPtr declarator_;
  };

  struct VariableDeclarationNode : public DeclarationNode {
    ASTNodePtr initializer_;

    VariableDeclarationNode( DeclaratorPtr declarator, ASTNodePtr initializer )
        : DeclarationNode( ASTNodeKind::VARIABLE_DECLARATION,
                           std::move( declarator ) )
        , initializer_( std::move( initializer ) ) {}

    bool operator==( const ASTNode& ) const override;
    void accept( ASTVisitor& visitor ) override { visitor.visit( *this ); }
  };

  struct FunctionDeclarationNode : public DeclarationNode {
    CompoundStatementNodePtr                   body_;
    std::vector< DeclarationStatementNodePtr > declaration_statement_list_;

    FunctionDeclarationNode(
        DeclaratorPtr                              declarator,
        std::vector< DeclarationStatementNodePtr > declaration_statement_list,
        CompoundStatementNodePtr                   body )
        : DeclarationNode( ASTNodeKind::FUNCTION_DECLARATION,
                           std::move( declarator ) )
        , declaration_statement_list_( std::move( declaration_statement_list ) )
        , body_( std::move( body ) ) {}

    FunctionDeclarationNode( DeclaratorPtr            declarator,
                             CompoundStatementNodePtr body )
        : DeclarationNode( ASTNodeKind::FUNCTION_DECLARATION,
                           std::move( declarator ) )
        , body_( std::move( body ) ) {}

    bool operator==( const ASTNode& ) const override;
    void accept( ASTVisitor& visitor ) override { visitor.visit( *this ); }
  };

  struct DeclarationStatementNode : public ASTNode {
    std::vector< DeclarationNodePtr > declaration_list_;

    DeclarationStatementNode(
        std::vector< DeclarationNodePtr > declaration_list )
        : ASTNode( ASTNodeKind::DECLARATION_STATEMENT )
        , declaration_list_( std::move( declaration_list ) ) {}

    bool operator==( const ASTNode& ) const override;
    void accept( ASTVisitor& visitor ) override { visitor.visit( *this ); }
  };

  struct GetAddressNode : public UnaryOperationNode {
    GetAddressNode( ASTNodePtr operand )
        : UnaryOperationNode( std::move( operand ), ASTNodeKind::GET_ADDRESS ) {
    }

    bool operator==( const ASTNode& ) const override;
  };

  struct ReturnNode : public ASTNode {
    ASTNodePtr result_;

    ReturnNode( ASTNodePtr operand )
        : ASTNode( ASTNodeKind::RETURN )
        , result_( std::move( operand ) ) {}

    bool operator==( const ASTNode& ) const override;
    void accept( ASTVisitor& visitor ) override { visitor.visit( *this ); }
  };

}  // namespace front
