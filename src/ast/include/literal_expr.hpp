#ifndef LITERAL_EXPR_HPP
#define LITERAL_EXPR_HPP

#include "expr.hpp"
#include "literal_kind.hpp"

class LiteralExpr : virtual public Expr {
private:
  LiteralKind kind;
  std::string lit;
public:
  LiteralExpr() = delete;
  LiteralExpr(AstNode* p, const std::string& l, LiteralKind lk) :
    Stmt(p), Expr(p), kind(lk), lit(l) {}
  virtual ~LiteralExpr() = default;

  LiteralKind getKind() const { return kind; }
  void setKind(LiteralKind lk) { kind = lk; }

  std::string& getLiteral() { return lit; }
  const std::string& getLiteral() const { return lit; }
  void setLiteral(const std::string& l) { lit = l; }
};

#endif
