#include "var_decl_stmt.hpp"

const std::vector<VarDecl*> VarDeclStmt::getVars() {
  std::vector<VarDecl*> ret;
  for(std::unique_ptr<VarDecl>& var : vars) {
    ret.push_back(var.get());
  }
  return ret;
}

const std::vector<const VarDecl*> VarDeclStmt::getVars() const {
  std::vector<const VarDecl*> ret;
  for(const std::unique_ptr<VarDecl>& var : vars) {
    ret.push_back(var.get());
  }
  return ret;
}
