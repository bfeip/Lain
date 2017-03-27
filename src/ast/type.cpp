#include "type.hpp"

int Type::nextId = 0;

const std::pair<Type*, AccessModifier>* Type::findParent(const std::string& name) {
  for(const std::pair<Type*, AccessModifier>& parent : parents) {
    if(parent.first->name == name) {
      return &parent;
    }
  }
  return nullptr;
}

const std::pair<Type*, AccessModifier>* Type::findChild(const std::string& name) {
  for(const std::pair<Type*, AccessModifier>& child : children) {
    if(child.first->name == name) {
      return &child;
    }
  }
  return nullptr;
}
