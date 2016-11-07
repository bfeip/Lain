#ifndef UNARY_OPERATION_EXPR_HPP
#define UNARY_OPERATION_EXPR_HPP

#include "ast_shared.hpp"
#include "expr.hpp"
#include "operation_type.hpp"

class UnaryOperationExpr : virtual public Expr {
private:
  std::unique_ptr<Expr> operand;
  OperationType op;
public:
  UnaryOperationExpr() = delete;
  UnaryOperationExpr(AstNode* owner, std::unique_ptr<Expr>&& expr, OperationType ot) :
    Stmt(owner), Expr(owner), operand(std::move(expr)), op(ot) {}
  virtual ~UnaryOperationExpr() = default;

  Expr* getOperand() { return operand.get(); }
  const Expr* getOperand() const { return operand.get(); }
  void setOperand(std::unique_ptr<Expr>&& expr) { operand = std::move(expr); }

  OperationType getOp() const { return op; }
  void setOp(OperationType ot) { op = ot; }
};

#endif
