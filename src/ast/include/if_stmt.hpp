#ifndef IF_STMT_HPP
#define IF_STMT_HPP

class IfStmt : virtual public Stmt, virtual public ScopeCreator {
private:
  std::unique_ptr<Expr> condition;
  std::unique_ptr<Stmt> body;
public:
  IfStmt() = delete;
  IfStmt(ScopeCreator* p) : Stmt(p), ScopeCreator(p) {}
  virtual ~IfStmt() = default;

  Expr* getCondition() { return condition.get(); }
  const Expr* getCondition() const { return condition.get(); }
  void setCondition(std::unique_ptr<Expr> con) { condition = std::move(con); }

  Stmt* getBody() { return body.get(); }
  const Stmt* getBody() const { return body.get(); }
  void setBody(std::unique_ptr<Stmt> b) { body = std::move(b); }
};

#endif
