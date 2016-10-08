#ifndef TYPE_HPP
#define TYPE_HPP

#include "ast_shared.hpp"

class Type {
private:
  static int nextId = 14;
  
  int id;
  bool broken;
  bool resolved;
  std::string name;
public:
  Type() : id(nextId++), broken(false), resolved(false), name("") {}
  Type(const std::string& name) : id(nextId++), broken(false), resolved(false), name(name) {}

  int getTypeId() const { return id; }
  bool isPrimitive() const { return id <= 13; }
  
  bool isBroken() const { return broken; }
  void setBroken() { broken = true; }

  bool isResolved() const { return resolved; }
  void setResolved() { resolved = true; }
  
  const std::string& getName() const { return name; }
  void setName(const std::string& name) { this->name = name; }
};

#endif
