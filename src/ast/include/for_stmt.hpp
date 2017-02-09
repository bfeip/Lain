#ifndef FOR_STMT_HPP
#define FOR_STMT_HPP

class ForStmt : virtual public Stmt, virtual public ScopeCreator {
private:
  std::unique_ptr<Stmt> start;
  std::unique_ptr<Expr> stop;
  std::unique_ptr<Expr> step;
  std::unique_ptr<CompoundStmt> body;
public:
  ForStmt() = delete;
  ForStmt(ScopeCreator* p) : Stmt(p), ScopeCreator(p) {}
  virtual ~ForStmt() = default;

  Stmt* getStart() { return start.get(); }
  const Stmt* getStart() const { return start.get(); }
  void setStart(std::unique_ptr<Stmt> stmt) { start = std::move(stmt); }

  Expr* getStop() { return stop.get(); }
  const Expr* getStop() const { return stop.get(); }
  void setStop(std::unique_ptr<Expr> expr) { stop = std::move(expr); }

  Expr* getStep() { return step.get(); }
  const Expr* getStep() const { return step.get(); }
  void setStep(std::unique_ptr<Expr> expr) { step = std::move(expr); }

  CompoundStmt* getBody() { return body.get(); }
  const CompoundStmt* getBody() const { return body.get(); }
  void setBody(std::unique_ptr<CompoundStmt> cs) { cs = std::move(cs); }
};

#endif
