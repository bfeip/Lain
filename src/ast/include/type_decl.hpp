#ifndef TYPE_DECL_HPP
#define TYPE_DECL_HPP

#include "ast_shared.hpp"
#include "access_modifier.hpp"
#include "type.hpp"

class TypeDecl : virtual public Decl {
private:
  std::unique_ptr<Type> type;
public:
  // change the constructor to instead of immedietly contructing a new type,
  // check the ast to see if the type is unresolved first
  TypeDecl() = delete;
  TypeDecl(ScopeCreator* sc) : Decl(sc) {}
  virtual ~TypeDecl() = default;
  
  void setName(std::unique_ptr<std::string>&& str) {
    name = std::move(str);
    type->setName(std::unique_ptr<std::string>(new std::string(*name.get())));
  }

  Type* getType() { return type.get(); }
  const Type* getType() const { return type.get(); }
};

#endif
