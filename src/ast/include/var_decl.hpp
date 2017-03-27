#ifndef VAR_DECL_HPP
#define VAR_DECL_HPP

#include "ast_shared.hpp"
#include "decl.hpp"
#include "qual_type.hpp"
#include "scope_creator.hpp"
class Expr;

class VarDecl : virtual public Decl {
private:
  std::unique_ptr<Expr> init;
  std::shared_ptr<QualType> type;
public:
  VarDecl() = delete;
  VarDecl(ScopeCreator* sc, std::shared_ptr<QualType> qt) :
    Decl(sc), type(qt) {}
  virtual ~VarDecl() = default;
  
  bool hasInit() const { return init != nullptr; }
  Expr* getInit() { return init.get(); }
  const Expr* getInit() const { return init.get(); }
  void setInit(std::unique_ptr<Expr> expr) { init = std::move(expr); }

  QualType* getType() { return type.get(); }
  const QualType* getType() const { return type.get(); }
  void setType(std::shared_ptr<QualType> qt) { type = qt; }
};

#include "expr.hpp"

#endif
