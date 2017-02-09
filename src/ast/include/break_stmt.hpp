#ifndef BREAK_STMT_HPP
#define BREAK_STMT_HPP

class BreakStmt : virtual public Stmt {
public:
  BreakStmt() = delete;
  BreakStmt(ScopeCreator* p) : Stmt(p) {}
  virtual ~BreakStmt() = default;
};

#endif
