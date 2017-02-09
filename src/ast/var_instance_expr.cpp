#include "var_instance_expr.hpp"

std::string* VarInstanceExpr::getName() {
  return var->getName();
}

const std::string* VarInstanceExpr::getName() const {
  return var->getName();
}
