#include "scope_creator.hpp"

const std::vector<FunctionDecl*> ScopeCreator::getFunctionDecls() {
  std::vector<FunctionDecl*> ret;
  for(std::pair<const std::unique_ptr<FunctionDecl>, AccessModifier>& fd : ownedFDecls) {
    ret.push_back(fd.first.get());
  }
  return ret;
}

const std::vector<TypeDecl*> ScopeCreator::getTypeDecls() {
  std::vector<TypeDecl*> ret;
  for(std::pair<const std::unique_ptr<TypeDecl>, AccessModifier>& td : ownedTDecls) {
    ret.push_back(td.first.get());
  }
  return ret;
}

FunctionDecl* ScopeCreator::findFunctionDecl(const std::string& name) {
  // breadth first(ish)
  // there is perhaps an error here for nameless functions
  for(std::pair<const std::unique_ptr<FunctionDecl>, AccessModifier>& fd : ownedFDecls) {
    if(*fd.first->getName() == name) {
      return fd.first.get();
    }
  }
  if(this == owner) {
    // this is the module (top-level)
    return nullptr;
  }
  else {
    return owner->findFunctionDecl(name);
  }
}

TypeDecl* ScopeCreator::findTypeDecl(const std::string& name) {
  // breadth first(ish)
  const std::vector<TypeDecl*> decls = getTypeDecls();
  for(TypeDecl* td : decls) {
    if(*td->getName() == name) {
      return td;
    }
  }
  if(this == owner) {
    // this is the module (top-level)
    return nullptr;
  }
  else {
    return owner->findTypeDecl(name);
  }
}

VarDecl* ScopeCreator::findVarDecl(const std::string& name) {
  for(VarDecl* var : ownedVars) {
    if(*var->getName() == name) {
      return var;
    }
  }
  if(owner == this) {
    return nullptr;
  }
  else {
    return owner->findVarDecl(name);
  }
}

Decl* ScopeCreator::findDecl(const std::string& name) {
  // I belive this introduces a bug where if there is a function and a type of the same name
  // the function will always be found first, even if the type is nearer in scope
  if(Decl* found = findFunctionDecl(name)) {
    return found;
  }
  else {
    return findTypeDecl(name);
  }
}

const std::vector<const VarDecl*> ScopeCreator::getVarDecls() const {
  std::vector<const VarDecl*> ret;
  for(const VarDecl* vd : ownedVars) {
    ret.push_back(vd);
  }
  return ret;
}

const std::vector<const FunctionDecl*> ScopeCreator::getFunctionDecls() const {
  std::vector<const FunctionDecl*> ret;
  for(const std::pair<const std::unique_ptr<FunctionDecl>, AccessModifier>& fd : ownedFDecls) {
    ret.push_back(fd.first.get());
  }
  return ret;
}

const std::vector<const TypeDecl*> ScopeCreator::getTypeDecls() const {
  std::vector<const TypeDecl*> ret;
  for(const std::pair<const std::unique_ptr<TypeDecl>, AccessModifier>& td : ownedTDecls) {
    ret.push_back(td.first.get());
  }
  return ret;
}

const FunctionDecl* ScopeCreator::findFunctionDecl(const std::string& name) const {
  // breadth first(ish)
  const std::vector<const FunctionDecl*> decls = getFunctionDecls();
  for(const FunctionDecl* fd : decls) {
    if(*fd->getName() == name) {
      return fd;
    }
  }
  if(this == owner) {
    // this is the module (top-level)
    return nullptr;
  }
  else {
    return owner->findFunctionDecl(name);
  }
}

const TypeDecl* ScopeCreator::findTypeDecl(const std::string& name) const {
  // breadth first(ish)
  const std::vector<const TypeDecl*> decls = getTypeDecls();
  for(const TypeDecl* td : decls) {
    if(*td->getName() == name) {
      return td;
    }
  }
  if(this == owner) {
    // this is the module (top-level)
    return nullptr;
  }
  else {
    return owner->findTypeDecl(name);
  }
}

const VarDecl* ScopeCreator::findVarDecl(const std::string& name) const {
  for(const VarDecl* var : ownedVars) {
    if(*var->getName() == name) {
      return var;
    }
  }
  if(owner == this) {
    return nullptr;
  }
  else {
    return owner->findVarDecl(name);
  }
}

const Decl* ScopeCreator::findDecl(const std::string& name) const {
  // I belive this introduces a bug where if there is a function and a type of the same name
  // the function will always be found first, even if the type is nearer in scope
  if(const Decl* found = findFunctionDecl(name)) {
    return found;
  }
  else {
    return findTypeDecl(name);
  }
}
