#ifndef VAR_DECL_HPP
#define VAR_DECL_HPP

#include "ast_shared.hpp"
#include "stmt.hpp"
#include "qual_type.hpp"

class VarDecl : public Decl {
private:
  std::unique_ptr<Stmt> init;
  QualType type;
public:
  VarDecl() : Decl(), init(nullptr) {}
  
  bool hasInit() const { return init != nullptr; }
  Stmt* getInit() const { return init.get(); }
  void setInit(std::unique_ptr<Stmt> init) { this->init = init; }

  QualType getType() const { return type; }
  void setType(const QualType& type) { this->type = type; }
};

#endif
