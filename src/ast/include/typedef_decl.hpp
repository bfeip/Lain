#ifndef TYPEDEF_DECL_HPP
#define TYPEDEF_DECL_HPP

#include "type_decl.hpp"

class TypeDefDecl : virtual public TypeDecl {
private:
  Type* definition;
public:
  TypeDefDecl() = delete;
  TypeDefDecl(ScopeCreator* sc) : Decl(sc), TypeDecl(sc) {}
  virtual ~TypeDefDecl() = default;

  Type* getDefinition() { return definition; }
  const Type* getDefinition() const { return definition; }
  void setDefinition(Type* def) { definition = def; }
};

#endif
