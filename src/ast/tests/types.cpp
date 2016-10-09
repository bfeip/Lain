#include "ast.hpp"

int main() {
  AST ast;
  std::unique_ptr AST_topLevelCD(new ClassDecl);
  ClassDecl* topLevelCD = AST_topLevelCD.get();

  ast.addTopDecl(std::move(AST_topLevelCD))
  topLevelCD.setName("test");

  CompoundStmt topLevelCDBody = std::unique_ptr(new CompoundStmt);
  topLevelCD.setBody(std::move(topLevelCDBody));

  return 0;
}
