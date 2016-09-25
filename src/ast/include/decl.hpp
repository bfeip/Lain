#ifndef DECL_HPP
#define DECL_HPP

#include "ast_shared.hpp"

class Decl {
private:
  bool broken;
  std::string name;
public:
  Decl() : broken(false), name("") {}
  bool isBroken() const { return broken; }
  void setBroken() { broken = true; }
  const std::string& getName() const { return name; }
  void setName(const std::string& name) { this->name = name; }
};

#endif
