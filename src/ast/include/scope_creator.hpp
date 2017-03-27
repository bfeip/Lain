#ifndef SCOPE_CREATOR_HPP
#define SCOPE_CREATOR_HPP

class ScopeCreator;
class Decl;
class FunctionDecl;
class Type;
class VarDecl;
class CompoundStmt;
#include "ast_shared.hpp"
#include "access_modifier.hpp"
#include "ast_node.hpp"

#include "llvm/ADT/StringMap.h"

class ScopeCreator : virtual public AstNode {
private:
  ScopeCreator* owner; // redundant in most cases but helpful
  
  std::unordered_map<std::unique_ptr<FunctionDecl>, AccessModifier> ownedFDecls;
  std::unordered_map<std::unique_ptr<Type>, AccessModifier> ownedTypes;
  std::vector<VarDecl*> ownedVars;
public:
  ScopeCreator() = delete;
  ScopeCreator(ScopeCreator* owner) : owner(owner) {}
  virtual ~ScopeCreator() = default;
  
  const std::vector<FunctionDecl*> getFunctionDecls();
  const std::vector<Type*> getTypes();
  const std::vector<VarDecl*>& getVarDecls() { return ownedVars; }
  
  FunctionDecl* findFunctionDecl(const std::string& name, bool external = false);
  Type* findType(const std::string& name, bool external = false);
  VarDecl* findVarDecl(const std::string& name, bool external = false);
  Decl* findDecl(const std::string& name);
  
  const std::vector<const FunctionDecl*> getFunctionDecls() const;   
  const std::vector<const Type*> getTypes() const;
  const std::vector<const VarDecl*> getVarDecls() const;
  
  const FunctionDecl* findFunctionDecl(const std::string& name, bool external = false) const;
  const Type* findType(const std::string& name, bool external = false) const;
  const VarDecl* findVarDecl(const std::string& name, bool external = false) const;
  const Decl* findDecl(const std::string& name) const;
  
  void addOwnedFunction(std::unique_ptr<FunctionDecl> fd, AccessModifier am) {
    ownedFDecls.emplace(std::move(fd), am);
  }
  void addOwnedType(std::unique_ptr<Type> t, AccessModifier am) {
    ownedTypes.emplace(std::move(t), am);
  }
  void addOwnedVar(VarDecl* vd) {
    ownedVars.push_back(vd);
  }
};

#include "function_decl.hpp"
#include "module.hpp"

#endif
