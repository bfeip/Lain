#ifndef ELSE_STMT_HPP
#define ELSE_STMT_HPP

class ElseStmt : virtual public Stmt, virtual public ScopeCreator {
private:
  std::unique_ptr<Stmt> body;
  IfStmt* ifStmt;
public:
  ElseStmt() = delete;
  ElseStmt(ScopeCreator* p) : Stmt(p), ScopeCreator(p) {}
  virtual ~ElseStmt() = default;
  
  Stmt* getBody() { return body.get(); }
  const Stmt* getBody() const { return body.get(); }
  void setBody(std::unique_ptr<Stmt>&& stmt) { body = std::move(stmt); }

  IfStmt* getIf() { return ifStmt; }
  const IfStmt* getIf() const { return ifStmt; }
  void setIf(IfStmt* is) { ifStmt = is; }
};

#endif
