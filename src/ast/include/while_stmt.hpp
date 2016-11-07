#ifndef WHILE_STMT_HPP
#define WHILE_STMT_HPP

class WhileStmt : virtual public Stmt {
private:
  std::unique_ptr<Expr> condition;
  std::unique_ptr<Stmt> body;
public:
  WhileStmt() = delete;
  WhileStmt(AstNode* p) : Stmt(p) {}
  virtual ~WhileStmt() = default;

  Expr* getCondition() { return condition.get(); }
  const Expr* getCondition() const { return condition.get(); }
  void setCondition(std::unique_ptr<Expr>&& expr) { condition = std::move(expr); }

  Stmt* getBody() { return body.get(); }
  const Stmt* getBody() const { return body.get(); }
  void setBody(std::unique_ptr<Stmt>&& s) { body = std::move(s); }
};

#endif
