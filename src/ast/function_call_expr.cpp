#include "function_call_expr.hpp"

std::vector<Expr*> FunctionCallExpr::getArgs() {
  std::vector<Expr*> ret;
  for(const std::unique_ptr<Expr>& arg : args) {
    ret.push_back(arg.get());
  }
  return ret;
}

std::vector<const Expr*> FunctionCallExpr::getArgs() const {
  std::vector<const Expr*> ret;
  for(const std::unique_ptr<Expr>& arg : args) {
    ret.push_back(arg.get());
  }
  return ret;
}
