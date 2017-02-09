#ifndef STMT_HPP
#define STMT_HPP

#include "ast_shared.hpp"
#include "ast_node.hpp"

class ScopeCreator;
class VarDecl;

class Stmt : virtual public AstNode {
private:
  ScopeCreator* owner;
public:
  Stmt() = delete;
  Stmt(ScopeCreator* p) : owner(p) {}
  virtual ~Stmt() = default;
  
  ScopeCreator* getOwner() { return owner; }
  const ScopeCreator* getOwner() const { return owner; }
  void setOwner(ScopeCreator* stmt) { owner = stmt; }
};

// more dep problems :/
#include "scope_creator.hpp"
#include "expr.hpp"
#include "var_decl.hpp"

#endif
