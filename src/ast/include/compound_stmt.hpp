#ifndef COMPOUND_STMT_HPP
#define COMPOUND_STMT_HPP

class TypeDecl;
#include "scope_creator.hpp"
#include "return_stmt.hpp"
#include "stmt.hpp"

class CompoundStmt : virtual public Stmt, virtual public ScopeCreator {
private:
  std::vector<std::unique_ptr<Stmt>> stmts;
public:
  CompoundStmt() = delete;
  CompoundStmt(ScopeCreator* p) : Stmt(p), ScopeCreator(p) {}
  virtual ~CompoundStmt() = default;

  const std::vector<Stmt*> getStmts();
  const std::vector<const Stmt*> getStmts() const;
  void addStmt(std::unique_ptr<Stmt>&& stmt);
};

#include "type_decl.hpp"

#endif
