#ifndef VAR_DECL_STMT_HPP
#define VAR_DECL_STMT_HPP

#include "stmt.hpp"

class VarDeclStmt : virtual public Stmt {
private:
  std::shared_ptr<QualType> type;
  std::vector<std::unique_ptr<VarDecl>> vars;
public:
  VarDeclStmt() = delete;
  VarDeclStmt(ScopeCreator* p) : Stmt(p) {}
  virtual ~VarDeclStmt() = default;

  QualType* getType() { return type.get(); }
  const QualType* getType() const { return type.get(); }
  void setType(std::shared_ptr<QualType> qt) { type = qt; }

  const std::vector<VarDecl*> getVars();
  const std::vector<const VarDecl*> getVars() const;
  std::vector<std::unique_ptr<VarDecl>>& stripVars() { return vars; }
  void addVar(std::unique_ptr<VarDecl> vd) { vars.emplace_back(std::move(vd)); }
};

#endif
