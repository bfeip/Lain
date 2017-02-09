#ifndef TYPE_HPP
#define TYPE_HPP

#include "ast_shared.hpp"
#include "access_modifier.hpp"
class TypeDecl;

class Type {
private:
  static int nextId;
  int id;
  
  std::unique_ptr<std::string> name;
  TypeDecl* decl;

  std::unordered_map<Type*, AccessModifier> parents;
  std::unordered_map<Type*, AccessModifier> children;
public:
  Type() = delete;
  Type(std::unique_ptr<std::string>&& str, TypeDecl* td) :
    id(nextId++), decl(td), name(std::move(str)) {}
  virtual ~Type() = default;

  int getTypeId() const { return id; }
  bool isPrimitive() const { return id <= 13; }

  bool isResolved() const { return decl != nullptr; }
  TypeDecl* getDecl() { return decl; }
  const TypeDecl* getDecl() const { return decl; }
  void setDecl(TypeDecl* td) { decl = td; }

  std::string* getName() { return name.get(); }
  const std::string* getName() const { return name.get(); }
  void setName(std::unique_ptr<std::string>&& str) { name = std::move(str); }
    
  const std::pair<Type*, AccessModifier>* findParent(const std::string& name);
  void addParent(Type* par, AccessModifier am) {
    parents.emplace(par, am);
  }

  const std::pair<Type*, AccessModifier>* findChild(const std::string& name);
  void addChild(Type* child, AccessModifier am) {
    children.emplace(child, am);
  }
};

#endif
