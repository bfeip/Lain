#ifndef CLASS_DECL_HPP
#define CLASS_DECL_HPP

#include "ast_shared.hpp"
#include "compund_stmt.hpp"
#include "member_var_decl.hpp"
#include "method_decl.hpp"

class ClassDecl : public TypeDecl {
private:
  std::unique_ptr<CompoundStmt> body;
  std::vector<MemberVarDecl> memberVars;
  std::vector<MethodDecl> methods;
public:
  ClassDecl() : TypeDecl(), body(nullptr) {}
  
  bool isDefined() const { return body != nullptr; }
  const CompoundStmt* getBody() const { return body.get() }
  void setBody(std::unique_ptr<CompundStmt> body) { this->body = body; }

  const std::vector<MemberVarDecl>& getMembers() const { return members; }
  void addMember(const MemberVarDecl& decl) { memberVars.push_back(decl); }
  void setMembers(const std::vector<MemberVarDecl>& decls) { memberVars = decls; }

  const std::vector<MemberVarDecl>& getMethods() const { return methods; }
  void addMethod(const MethodDecl& decl) { methods.push_back(decl); }
  void setMethods(const std::vector<MethodDecl>& decls) { methods = decls; }
};

#endif
