#include "function_decl.hpp"

const std::vector<VarDecl*> FunctionDecl::getParams() {
  std::vector<VarDecl*> ret;
  for(const std::unique_ptr<VarDecl>& param : params) {
    ret.push_back(param.get());
  }
  return ret;
}

const std::vector<const VarDecl*> FunctionDecl::getParams() const {
  std::vector<const VarDecl*> ret;
  for(const std::unique_ptr<VarDecl>& param : params) {
    ret.push_back(param.get());
  }
  return ret;
}
