#ifndef RETURN_STMT_HPP
#define RETURN_STMT_HPP

#include "stmt.hpp"
#include "scope_creator.hpp"
#include "expr.hpp"

class ReturnStmt : virtual public Stmt, virtual public ScopeCreator /* ikr */ {
private:
  std::unique_ptr<Expr> value;
public:
  ReturnStmt() = delete;
  ReturnStmt(ScopeCreator* p) : Stmt(p), ScopeCreator(p) {}
  virtual ~ReturnStmt() = default;

  Expr* getValue() { return value.get(); }
  const Expr* getValue() const { return value.get(); }
  void setValue(std::unique_ptr<Expr>&& expr) { value = std::move(expr); }
};

#endif
