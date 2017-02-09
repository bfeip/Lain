#include "compound_stmt.hpp"

const std::vector<Stmt*> CompoundStmt::getStmts() {
  std::vector<Stmt*> ret;
  for(std::unique_ptr<Stmt>& stmt : stmts) {
    ret.push_back(stmt.get());
  }
  return ret;
}

const std::vector<const Stmt*> CompoundStmt::setStmts() const {
    std::vector<const Stmt*> ret;
    for(const std::unique_ptr<Stmt>& stmt : stmts) {
      ret.push_back(stmt.get());
    }
    return ret;
}
