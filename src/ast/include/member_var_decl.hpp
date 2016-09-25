#ifndef MEMBER_VAR_DECL_HPP
#define MEMBER_VAR_DECL_HPP

#include "ast_shared.hpp"
#include "class_decl.hpp"

class MemberVarDecl : public VarDecl {
private:
  const ClassDecl* parent;
public:
  MemberVarDecl() : VarDecl() {}

  const ClassDecl* getParent() const { return parent; }
  void setParent(const ClassDecl* parent) { this->parent = parent; }
};

#endif
