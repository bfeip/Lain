#ifndef EXPR_HPP
#define EXPR_HPP

#include "qual_type.hpp"
#include "stmt.hpp"

class Expr : virtual public Stmt {
private:
  std::unique_ptr<QualType> type;

  Expr* parentExpr;
public:
  Expr() = delete;
  Expr(ScopeCreator* owner, Expr* pe) : Stmt(owner), parentExpr(pe) {}
  virtual ~Expr() = default;
  
  QualType* getType() { return type.get(); };
  const QualType* getType() const { return type.get(); }
  void setType(std::unique_ptr<QualType> qt) { type = std::move(qt); }
};

#endif
