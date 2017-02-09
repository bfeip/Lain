#ifndef BINARY_OPERATION_EXPR_HPP
#define BINARY_OPERATION_EXPR_HPP

#include "ast_shared.hpp"
#include "expr.hpp"
#include "operation_type.hpp"

class BinaryOperationExpr : virtual public Expr {
private:
  std::unique_ptr<Expr> lOperand;
  std::unique_ptr<Expr> rOperand;
  OperationType op;
public:
  BinaryOperationExpr() = delete;
  BinaryOperationExpr(ScopeCreator* owner, Expr* pe, std::unique_ptr<Expr> l,
		      std::unique_ptr<Expr> r, OperationType ot) :
    Stmt(owner), Expr(owner, pe), lOperand(std::move(l)), rOperand(std::move(r)), op(ot) {}
  virtual ~BinaryOperationExpr() = default;

  Expr* getLeftOperand() { return lOperand.get(); }
  const Expr* getLeftOperand() const { return lOperand.get(); }
  void setLeftOperand(std::unique_ptr<Expr>&& expr) { lOperand = std::move(expr); }

  Expr* getRightOperand() { return rOperand.get(); }
  const Expr* getRightOperand() const { return rOperand.get(); }
  void setRightOperand(std::unique_ptr<Expr>&& expr) { rOperand = std::move(expr); }

  OperationType getOp() const { return op; }
  void setOp(OperationType ot) { op = ot; }
};

#endif
