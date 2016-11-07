#ifndef RETURN_STMT_HPP
#define RETURN_STMT_HPP

class ReturnStmt : virtual public Stmt {
private:
  std::unique_ptr<Expr> value;
public:
  ReturnStmt() = delete;
  ReturnStmt(AstNode* p) : Stmt(p) {}
  virtual ~ReturnStmt() = default;

  Expr* getValue() { return value.get(); }
  const Expr* getValue() const { return value.get(); }
  void setValue(std::unique_ptr<Expr>&& expr) { value = std::move(expr); }
};

#endif
