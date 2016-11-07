#ifndef CLASS_DECL_HPP
#define CLASS_DECL_HPP

class ClassDecl;
class MemberVarDecl;
#include "ast_shared.hpp"
#include "compound_stmt.hpp"
#include "type_decl.hpp"
#include "access_modifier.hpp"

class ClassDecl : virtual public TypeDecl, virtual public ScopeCreator  {
private:
  // ScopeCreator contains the ownedFDecls and ownedTDecls members and
  // relivent methods
  std::unordered_map<std::unique_ptr<VarDecl>, AccessModifier> memberVars;
public:
  ClassDecl() = delete;
  ClassDecl(ScopeCreator* sc) : Decl(sc), TypeDecl(sc), ScopeCreator() {}
  virtual ~ClassDecl() = default;

  const std::pair<VarDecl*, AccessModifier>& findMember(const std::string& name);
  const std::pair<const VarDecl*, AccessModifier>& findMember(const std::string& name) const;
  void addMember(std::unique_ptr<VarDecl>&& decl, AccessModifier am) {
    memberVars.emplace(std::move(decl), am);
  }
};

#endif
