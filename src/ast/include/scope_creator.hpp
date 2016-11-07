#ifndef SCOPE_CREATOR_HPP
#define SCOPE_CREATOR_HPP

class ScopeCreator;
class FunctionDecl;
class TypeDecl;
#include "access_modifier.hpp"
#include "ast_node.hpp"

class ScopeCreator : virtual public AstNode {
private:
  std::unordered_map<std::unique_ptr<FunctionDecl>, AccessModifier> ownedFDecls;
  std::unordered_map<std::unique_ptr<TypeDecl>, AccessModifier> ownedTDecls;
public:
  ScopeCreator() = default;
  virtual ~ScopeCreator() = default;
  
  const std::vector<FunctionDecl*>& getFunctionDecls();
  const std::vector<TypeDecl*>& getTypeDecls();
  FunctionDecl* findFunctionDecl(const std::string& name);
  TypeDecl* findTypeDecl(const std::string& name);
  const std::vector<const FunctionDecl*>& getFunctionDecls() const;
  const std::vector<const TypeDecl*>& getTypeDecls() const;
  const FunctionDecl* findFunctionDecl(const std::string& name) const;
  const TypeDecl* findTypeDecl(const std::string& name) const;
  
  void addOwnedFunction(std::unique_ptr<FunctionDecl>&& fd, AccessModifier am) { ownedFDecls.emplace(std::move(fd), am); }
  void addOwnedType(std::unique_ptr<TypeDecl>&& td, AccessModifier am) { ownedTDecls.emplace(std::move(td), am); }
};

// this has to be done because of funky circular deps
#include "function_decl.hpp"
#include "type_decl.hpp"

#endif
