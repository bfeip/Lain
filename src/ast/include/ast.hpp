#ifndef AST_HPP
#define AST_HPP

#include "ast_shared.hpp"
#include "decl.hpp"
#include "type.hpp"

class AST {
private:
  std::vector<std::unique_ptr<Decl>> decls;
  std::vector<std::unique_ptr<Type>> types;
public:
  AST() = default;
  ~AST() = default;
  void addTopLevelDecl(std::unique_ptr<Decl> decl);
  void addTopLevelType(std::unique_ptr<Type> type);
  const std::vector<Decl*>& getTopLevelDecls() const;
  const std::vector<Type*>& getTopLevelTypes() const;
};

#endif
