#ifndef STMT_HPP
#define STMT_HPP

#include "ast_shared.hpp"

class VarDecl;

class Stmt : virtual public AstNode {
private:
  AstNode* owner;
public:
  Stmt() = delete;
  Stmt(AstNode* p) : owner(p) {}
  virtual ~Stmt() = default;
  
  AstNode* getOwner() { return owner; }
  const AstNode* getOwner() const { return owner; }
  void setOwner(AstNode* stmt) { owner = stmt; }

  virtual VarDecl* findVar(const std::string& str);
  ScopeCreator* findScope();
};

// more dep problems :/
#include "expr.hpp"
#include "var_decl.hpp"

#endif
