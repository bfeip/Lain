#ifndef SCOPE_CREATOR_HPP
#define SCOPE_CREATOR_HPP

class ScopeCreator;
class Decl;
class FunctionDecl;
class TypeDecl;
class VarDecl;
class CompoundStmt;
#include "ast_shared.hpp"
#include "access_modifier.hpp"
#include "ast_node.hpp"

class ScopeCreator : virtual public AstNode {
private:
  ScopeCreator* owner; // redundant in most cases but helpful
  
  std::unordered_map<std::unique_ptr<FunctionDecl>, AccessModifier> ownedFDecls;
  std::unordered_map<std::unique_ptr<TypeDecl>, AccessModifier> ownedTDecls;
  std::vector<VarDecl*> ownedVars;
public:
  ScopeCreator() = delete;
  ScopeCreator(ScopeCreator* owner) : owner(owner) {}
  virtual ~ScopeCreator() = default;
  
  const std::vector<FunctionDecl*> getFunctionDecls();
  const std::vector<TypeDecl*> getTypeDecls();
  const std::vector<VarDecl*>& getVarDecls() { return ownedVars; }
  
  FunctionDecl* findFunctionDecl(const std::string& name);
  TypeDecl* findTypeDecl(const std::string& name);
  VarDecl* findVarDecl(const std::string& name);
  Decl* findDecl(const std::string& name);
  
  const std::vector<const FunctionDecl*> getFunctionDecls() const;   
  const std::vector<const TypeDecl*> getTypeDecls() const;
  const std::vector<const VarDecl*> getVarDecls() const;
  
  const FunctionDecl* findFunctionDecl(const std::string& name) const;
  const TypeDecl* findTypeDecl(const std::string& name) const;
  const VarDecl* findVarDecl(const std::string& name) const;
  const Decl* findDecl(const std::string& name) const;
  
  void addOwnedFunction(std::unique_ptr<FunctionDecl>&& fd, AccessModifier am) {
    ownedFDecls.emplace(std::move(fd), am);
  }
  void addOwnedType(std::unique_ptr<TypeDecl>&& td, AccessModifier am) {
    ownedTDecls.emplace(std::move(td), am);
  }
  void addOwnedVar(VarDecl* vd) {
    ownedVars.push_back(vd);
  }
};

#include "function_decl.hpp"

#endif
