#ifndef AST_HPP
#define AST_HPP

#include "ast_shared.hpp"
#include "decl.hpp"
#include "type.hpp"

class AST {
private:
  std::vector<std::unique_ptr<Decl>> topDecls;
  ScopeTree types; /* tree of TU types, a node can only "see" its immediate children, its siblings,
		    * any of its parent nodes, or any top level node */
  Decl* curDecl;
public:
  AST() = default;

  void proccessDecl(std::unique_ptr<Decl> decl);
  void endDecl();

  /* Types are added when a TypeDecl (or function, because functions create a type scope)
   * is pushed */

  Decl* findDecl(const std::string& name); // curDecl is used as scope
  Type* findType(const std::string& name);

  void resolvePendingTypes();
};

#endif
