#ifndef VAR_INSTANCE_EXPR_HPP
#define VAR_INSTANCE_EXPR_HPP

#include "ast_shared.hpp"
#include "stmt.hpp"
#include "expr.hpp"
#include "var_decl.hpp"

class VarInstanceExpr : virtual public Expr {
private:
  VarDecl* var;
public:
  VarInstanceExpr() = delete;
  VarInstanceExpr(ScopeCreator* owner, Expr* pe, VarDecl* vd) :
    Stmt(owner), Expr(owner, pe), var(vd) {}
  virtual ~VarInstanceExpr() = default;

  VarDecl* getVar() { return var; }
  const VarDecl* getVar() const { return var; }
  void setVar(VarDecl* vd) { var = vd; }

  std::string* getName();
  const std::string* getName() const;
};

#endif
