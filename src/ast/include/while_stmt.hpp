#ifndef WHILE_STMT_HPP
#define WHILE_STMT_HPP

#include "scope_creator.hpp"
#include "expr.hpp"

class WhileStmt : virtual public Stmt, virtual public ScopeCreator {
private:
  std::unique_ptr<Expr> condition;
  std::unique_ptr<Stmt> body;
public:
  WhileStmt() = delete;
  WhileStmt(ScopeCreator* p) : Stmt(p), ScopeCreator(p) {}
  virtual ~WhileStmt() = default;

  Expr* getCondition() { return condition.get(); }
  const Expr* getCondition() const { return condition.get(); }
  void setCondition(std::unique_ptr<Expr>&& expr) { condition = std::move(expr); }

  Stmt* getBody() { return body.get(); }
  const Stmt* getBody() const { return body.get(); }
  void setBody(std::unique_ptr<Stmt>&& s) { body = std::move(s); }
};

#endif
