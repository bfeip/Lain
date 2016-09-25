#ifndef TYPE_HPP
#define TYPE_HPP

#include "ast_shared.hpp"

class Type {
private:
  bool broken;
  std::string name;
public:
  Type() : broken(false), name("") {}
  Type(const std::string& name) : broken(false), name(name) {}
  
  bool isBroken() const { return broken; }
  void setBroken() { broken = true; }
  
  const std::string& getName() const { return name; }
  void setName(const std::string& name) { this->name = name; }
};

#endif
