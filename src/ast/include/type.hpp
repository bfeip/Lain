#ifndef TYPE_HPP
#define TYPE_HPP

#include "ast_shared.hpp"
#include "access_modifier.hpp"
class TypeDecl;

class Type {
private:
  static int nextId;
  int id;
  
  std::string name;
  std::unique_ptr<TypeDecl> decl;

  std::unordered_map<Type*, AccessModifier> parents;
  std::unordered_map<Type*, AccessModifier> children;
public:
  Type() = delete;
  Type(const std::string& str, std::unique_ptr<TypeDecl> td) :
    id(nextId++), decl(std::move(td)), name(str) {}
  virtual ~Type() = default;

  int getTypeId() const { return id; }
  bool isPrimitive() const { return id <= 13; }

  bool isResolved() const { return decl != nullptr; }
  TypeDecl* getDecl() { return decl.get(); }
  const TypeDecl* getDecl() const { return decl.get(); }
  void setDecl(std::unique_ptr<TypeDecl> td) { decl = std::move(td); }

  std::string& getName() { return name; }
  const std::string& getName() const { return name; }
  void setName(const std::string& str) { name = str; }
    
  const std::pair<Type*, AccessModifier>* findParent(const std::string& name);
  void addParent(Type* par, AccessModifier am) {
    parents.emplace(par, am);
  }

  const std::pair<Type*, AccessModifier>* findChild(const std::string& name);
  void addChild(Type* child, AccessModifier am) {
    children.emplace(child, am);
  }
};

#include "type_decl.hpp"

#endif
