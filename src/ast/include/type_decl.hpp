#ifndef TYPE_DECL_HPP
#define TYPE_DECL_HPP

class Type;
#include "ast_shared.hpp"
#include "decl.hpp"
#include "access_modifier.hpp"

class TypeDecl : virtual public Decl {
private:
  std::unique_ptr<Type> type;
public:
  // change the constructor to instead of immedietly contructing a new type,
  // check the ast to see if the type is unresolved first
  TypeDecl() = delete;
  TypeDecl(ScopeCreator* sc) : Decl(sc) {}
  virtual ~TypeDecl() = default;
  
  void setName(std::unique_ptr<std::string> str);
  Type* getType() { return type.get(); }
  const Type* getType() const { return type.get(); }
};

#include "type.hpp"

#endif
