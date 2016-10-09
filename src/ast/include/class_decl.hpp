#ifndef CLASS_DECL_HPP
#define CLASS_DECL_HPP

#include "ast_shared.hpp"
#include "compund_stmt.hpp"
#include "member_var_decl.hpp"
#include "method_decl.hpp"

class ClassDecl : public TypeDecl {
private:
  std::unique_ptr<CompoundStmt> body;
  
  std::unordered_map<std::unique_ptr<MemberVarDecl>, AccessModifier> memberVars;
  std::unordered_map<std::unique_ptr<MethodDecl>, AccessModifier> methods;
  
  std::unordered_map<ClassDecl*, AccessModifier> parents;
  std::unordered_map<ClassDecl*, AccessModifier> children;
  
  std::unordered_map<std::unique_ptr<TypeDecl>, AccessModifier> ownedTypes;
  std::pair<Decl*, AccessModifer> owner;
public:
  ClassDecl() : TypeDecl(), body(nullptr) {}
  
  bool isDefined() const { return body != nullptr; }
  const CompoundStmt* getBody() const { return body.get() }
  void setBody(std::unique_ptr<CompundStmt> body) { this->body = body; setResolved(); }

  const std::pair<const MemberVarDecl*, AccessModifier>& findMember(const std::string& name) const;
  void addMember(const MemberVarDecl& decl, AccessModifier am) { memberVars.insert(decl, am); }

  const std::pair<const MethodDecl*, AccessModifier>& findMethod(const std::string& name) const;
  void addMethod(const MethodDecl& decl) { methods.push_back(decl); }

  const std::pair<const ClassDecl*, AccessModifer>& findParent(const std::string& name) const;
  void addParent(const ClassDecl* par, AccessModifier am) { parents.insert(par, am); }

  const std::pair<const ClassDecl*, AccessModifer>& findChild(const std::string& name) const;
  void addChild(const ClassDecl* child, AccessModifier am) { children.insert(child, am); }

  const std::pair<const TypeDecl*, AccessModifier>& findContainedType(const std::string& name) const;
  void addContainedType(const TypeDecl* type, AccessModifier am) { containedTypes.insert(type, am); }
};

#endif
