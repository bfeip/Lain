#ifndef FUNCTION_DECL_HPP
#define FUNCTION_DECL_HPP

#include "ast_shared.hpp"
#include "scope_creator.hpp"
#include "qual_type.hpp"
#include "var_decl.hpp"
#include "compound_stmt.hpp"

class FunctionDecl : virtual public Decl, virtual public ScopeCreator {
private:
  std::unique_ptr<QualType> returnType;
  std::vector<std::unique_ptr<VarDecl>> params;
  std::unique_ptr<CompoundStmt> body;
public:
  FunctionDecl() = delete;
  FunctionDecl(ScopeCreator* sc) : Decl(sc), ScopeCreator(sc) {}
  virtual ~FunctionDecl() = default;
  
  bool isDefined() const { return body != nullptr; }
  
  QualType* getReturnType() { return returnType.get(); }
  const QualType* getReturnType() const { return returnType.get(); }
  void setReturnType(std::unique_ptr<QualType> qt) { returnType = std::move(qt); }

  const std::vector<VarDecl*> getParams();
  const std::vector<const VarDecl*> getParams() const;
  void addParam(std::unique_ptr<VarDecl> param) { params.emplace_back(std::move(param)); }
  
  CompoundStmt* getBody() { return body.get(); }
  const CompoundStmt* getBody() const { return body.get(); }
  void setBody(std::unique_ptr<CompoundStmt> cs) { body = std::move(body); }
};

#endif
