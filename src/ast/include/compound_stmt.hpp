#ifndef COMPOUND_STMT_HPP
#define COMPOUND_STMT_HPP

#include "ast_shared.hpp"
#include "scope_creator.hpp"
#include "stmt.hpp"

class CompoundStmt : virtual public Stmt, virtual public ScopeCreator {
private:
  std::vector<std::unique_ptr<Stmt>> stmts;
public:
  CompoundStmt() = delete;
  CompoundStmt(AstNode* p) : Stmt(p), ScopeCreator() {}
  virtual ~CompoundStmt() = default;

  const std::vector<Stmt*>& getStmts();
  const std::vector<const Stmt*>& setStmts() const;
  void addStmt(std::unique_ptr<Stmt> stmt) { stmts.emplace_back(std::move(stmt)); }
};

#endif
