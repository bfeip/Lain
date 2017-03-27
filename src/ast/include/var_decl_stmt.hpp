#ifndef VAR_DECL_STMT_HPP
#define VAR_DECL_STMT_HPP

#include "stmt.hpp"

class VarDeclStmt : virtual public Stmt, virtual public ScopeCreator {
private:
  std::shared_ptr<QualType> type;
  std::vector<std::unique_ptr<VarDecl>> vars;
public:
  VarDeclStmt() = delete;
  VarDeclStmt(ScopeCreator* p) : Stmt(p), ScopeCreator(p) {}
  virtual ~VarDeclStmt() = default;

  QualType* getType() { return type.get(); }
  const QualType* getType() const { return type.get(); }
  void setType(std::shared_ptr<QualType> qt) { type = qt; }

  const std::vector<VarDecl*> getVars();
  const std::vector<const VarDecl*> getVars() const;
  std::vector<std::unique_ptr<VarDecl>>&& stripVars() { return std::move(vars); }
  void addVar(std::unique_ptr<VarDecl> vd) { vars.push_back(std::move(vd)); }
};

#endif
