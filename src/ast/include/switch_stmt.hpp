#ifndef SWITCH_STMT_HPP
#define SWITCH_STMT_HPP

class SwitchStmt : virtual public Stmt, virtual public ScopeCreator {
private:
  std::unique_ptr<Expr> condition;
  std::unique_ptr<CompoundStmt> body;
public:
  SwitchStmt() = delete;
  SwitchStmt(ScopeCreator* p) : Stmt(p), ScopeCreator(p) {}
  virtual ~SwitchStmt() = default;

  Expr* getCondition() { return condition.get(); }
  const Expr* getContion() const { return condition.get(); }
  void setCondition(std::unique_ptr<Expr>&& expr) { condition = std::move(expr); }

  CompoundStmt* getBody() { return body.get(); }
  const CompoundStmt* getBody() const { return body.get(); }
  void setBody(std::unique_ptr<CompoundStmt>&& cs) { body = std::move(cs); }
};

#endif
