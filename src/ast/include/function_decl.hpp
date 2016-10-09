#ifndef FUNCTION_DECL_HPP
#define FUNCTION_DECL_HPP

#include "ast_shared.hpp"
#include "qual_type.hpp"
#include "var_decl.hpp"
#include "compound_stmt.hpp"

class FunctionDecl : public Decl {
private:
  QualType returnType;
  std::vector<VarDecl> params;
  std::unique_ptr<CompoundStmt> body;
  
  Decl* owner;
  std::vector<std::unique_ptr<Decl>> owned;
public:
  FunctionDecl() : Decl(), body(nullptr) {}
  bool isDefined() const { return body != nullptr; }
  
  const QualType& getReturnType() const { return returnType; }
  void setReturnType(const QualType& returnType) { this->returnType = returnType; }

  const std::vector<VarDecl>& getParams() const { return params; }
  void setParams(const std::vector<VarDecl>& params) { this->params = params; }
  void addParam(const VarDecl& param) { params.push_back(param); }
  
  const CompoundStmt* getBody() const { return body.get(); }
  void setBody(std::unique_ptr<CompoundStmt> body) { this->body = body; }

  Decl* getOwner() { return owner; }
  void setOwner(Decl* owner) { this->owner = owner; }

  std::vector<Decl*> getOwned() { return owned; }
  void addOwned(Decl* d) { owned.push_back(d); }
};

#endif
