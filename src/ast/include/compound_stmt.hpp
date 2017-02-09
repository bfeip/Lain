#ifndef COMPOUND_STMT_HPP
#define COMPOUND_STMT_HPP

#include "type_decl.hpp"
#include "scope_creator.hpp"
#include "stmt.hpp"

class CompoundStmt : virtual public Stmt, virtual public ScopeCreator {
private:
  std::vector<std::unique_ptr<Stmt>> stmts;
public:
  CompoundStmt() = delete;
  CompoundStmt(ScopeCreator* p) : Stmt(p), ScopeCreator(p) {}
  virtual ~CompoundStmt() = default;

  const std::vector<Stmt*> getStmts();
  const std::vector<const Stmt*> setStmts() const;
  void addStmt(std::unique_ptr<Stmt> stmt) { stmts.emplace_back(std::move(stmt)); }
};

#endif
