#include "include/ast.hpp"

void AST::addTopLevelDecl(std::unique_ptr<Decl> decl) {
  decls.push_back(std::move(decl));
  return;
}

void AST::addTopLevelType(std::unique_ptr<Type> type) {
  types.push_back(std::move(type));
  return;
}

std::vector<Decl*> AST::getTopLevelDecls() const {
  std::vector<Decl*> ret;
  for(auto& e : decls) {
    ret.push_back(e.get());
  }
  return ret;
}

std::vector<Type*> AST::getTopLevelTypes() const {
  std::vector<Type*> ret;
  for(auto& e : types) {
    ret.push_back(e.get());
  }
  return ret;
}
