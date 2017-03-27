#ifndef TYPE_DECL_HPP
#define TYPE_DECL_HPP

class Type;
#include "decl.hpp"

class TypeDecl : virtual public Decl {
private:
  Type* type;
public:
  // change the constructor to instead of immedietly contructing a new type,
  // check the ast to see if the type is unresolved first
  TypeDecl() = delete;
  TypeDecl(ScopeCreator* sc) : Decl(sc) {}
  virtual ~TypeDecl() = default;
  
  void setName(std::unique_ptr<std::string> str);
  Type* getType() { return type; }
  const Type* getType() const { return type; }
};

#include "type.hpp"
#include "ast_shared.hpp"
#include "access_modifier.hpp"


#endif
