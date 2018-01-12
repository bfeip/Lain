#ifndef FUNCTION_CALL_EXPR_HPP
#define FUNCTION_CALL_EXPR_HPP

class FunctionDecl;
#include "expr.hpp"

class FunctionCallExpr : virtual public Expr {
private:
  FunctionDecl* func;
  std::vector<std::unique_ptr<Expr>> args;

  std::string funcName;
public:
  FunctionCallExpr() = delete;
  FunctionCallExpr(ScopeCreator* owner, Expr* pe, FunctionDecl* fd) :
    Stmt(owner), Expr(owner, pe), func(fd) {}
  virtual ~FunctionCallExpr() = default;

  FunctionDecl* getFunc() { return func; }
  const FunctionDecl* getFunc() const { return func; }
  void setFunc(FunctionDecl* fd) { func = fd; }

  std::vector<Expr*> getArgs();
  std::vector<const Expr*> getArgs() const;
  void addArg(std::unique_ptr<Expr> arg) { args.emplace_back(std::move(arg)); }

  std::string& getFuncName() { return funcName; }
  const std::string& getFuncName() const { return funcName; }
  void setFuncName(const std::string& s) { funcName = s; }
};

#include "function_decl.hpp"

#endif
