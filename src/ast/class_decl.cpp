#include "class_decl.hpp"

const std::pair<VarDecl*, AccessModifier> ClassDecl::findMember(const std::string& name) {
  std::unordered_map<std::unique_ptr<VarDecl>, AccessModifier>::iterator it = memberVars.begin();
  while(it != memberVars.end()) {
    if(*it->first->getName() == name) {
      std::pair<VarDecl*, AccessModifier> ret(it->first.get(), it->second);
      return ret;
    }
  }
}

const std::pair<const VarDecl*, AccessModifier> ClassDecl::findMember(const std::string& name) const {
  std::unordered_map<std::unique_ptr<VarDecl>, AccessModifier>::const_iterator it = memberVars.cbegin();
  while(it != memberVars.cend()) {
    if(*it->first->getName() == name) {
      std::pair<const VarDecl*, AccessModifier> ret(it->first.get(), it->second);
      return ret;
    }
  }
}
