#ifndef METHOD_DECL_HPP
#define METHOD_DECL_HPP

#include "ast_shared.hpp"
#include "class_decl.hpp"

class MethodDecl : public FunctionDecl {
private:
  const ClassDecl* parent;
public:
  MethodDecl() : FunctionDecl() {}

  const ClassDecl* getParent() const { return parent; }
  void setParent(const ClassDecl* parent) { this->parent = parent; }
};

#endif
