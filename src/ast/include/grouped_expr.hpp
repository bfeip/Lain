#ifndef GROUPED_EXPR_HPP
#define GROUPED_EXPR_HPP

#include "ast_shared.hpp"
#include "expr.hpp"

class GroupedExpr : virtual public Expr {
private:
  std::unique_ptr<Expr> body;
public:
  GroupedExpr() = delete;
  GroupedExpr(ScopeCreator* owner, Expr* pe, std::unique_ptr<Expr> expr) :
    Stmt(owner), Expr(owner, pe), body(std::move(expr)) {}
  virtual ~GroupedExpr() = default;

  Expr* getBody() { return body.get(); }
  const Expr* getBody() const { return body.get(); }
  void setBody(std::unique_ptr<Expr>&& expr) { body = std::move(expr); }
};

#endif
