#ifndef TYPE_DECL_HPP
#define TYPE_DECL_HPP

#include "ast_shared.hpp"

class TypeDecl : public Decl {
private:
  std::unique_ptr<Type> type;
public:
  TypeDecl() : type(new type()) {}
  
  void setName(const std::string& name) { this->name = name; type->setName(name); }
};

#endif
