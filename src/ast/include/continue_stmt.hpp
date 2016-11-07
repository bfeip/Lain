#ifndef CONTINUE_STMT_HPP
#define CONTINUE_STMT_HPP

class ContinueStmt : public Stmt {
public:
  ContinueStmt() = delete;
  ContinueStmt(AstNode* p) : Stmt(p) {}
  virtual ~ContinueStmt() = default;
};

#endif
