#include "scope_creator.hpp"

const std::vector<FunctionDecl*> ScopeCreator::getFunctionDecls() {
  std::vector<FunctionDecl*> ret;
  for(std::pair<const std::unique_ptr<FunctionDecl>, AccessModifier>& fd : ownedFDecls) {
    ret.push_back(fd.first.get());
  }
  return ret;
}

const std::vector<Type*> ScopeCreator::getTypes() {
  std::vector<Type*> ret;
  for(std::pair<const std::unique_ptr<Type>, AccessModifier>& t : ownedTypes) {
    ret.push_back(t.first.get());
  }
  return ret;
}

FunctionDecl* ScopeCreator::findFunctionDecl(const std::string& name, bool external) {
  // breadth first(ish)
  // there is perhaps an error here for nameless functions
  for(std::pair<const std::unique_ptr<FunctionDecl>, AccessModifier>& fd : ownedFDecls) {
    if(*fd.first->getName() == name) {
      return fd.first.get();
    }
  }
  if(this == owner) {
    // this is the module (top-level)
    if(external) {
      // this was call came from an outside module stop here
      return nullptr;
    }
    Module* module = dynamic_cast<Module*>(this);
    for(llvm::StringMapEntry<std::unique_ptr<Module>>& ex : module->getExternals()) {
      FunctionDecl* fd = ex.getValue()->findFunctionDecl(name, true);
      if(fd) {
	return fd;
      }
    }
    return nullptr;
  }
  else {
    return owner->findFunctionDecl(name);
  }
}

Type* ScopeCreator::findType(const std::string& name, bool external) {
  // breadth first(ish)
  const std::vector<Type*> types = getTypes();
  for(Type* t : types) {
    if(t->getName() == name) {
      return t;
    }
  }
  if(this == owner) {
    // this is the module (top-level)
    if(external) {
      // this was call came from an outside module stop here
      return nullptr;
    }
    Module* module = dynamic_cast<Module*>(this);
    for(llvm::StringMapEntry<std::unique_ptr<Module>>& ex : module->getExternals()) {
      Type* t = ex.getValue()->findType(name, true);
      if(t) {
	return t;
      }
    }
    return nullptr;
  }
  else {
    return owner->findType(name);
  }
}

VarDecl* ScopeCreator::findVarDecl(const std::string& name, bool external) {
  for(VarDecl* var : ownedVars) {
    if(*var->getName() == name) {
      return var;
    }
  }
  if(owner == this) {
    // this is the module (top-level)
    if(external) {
      // this was call came from an outside module stop here
      return nullptr;
    }
    Module* module = dynamic_cast<Module*>(this);
    for(llvm::StringMapEntry<std::unique_ptr<Module>>& ex : module->getExternals()) {
      VarDecl* vd = ex.getValue()->findVarDecl(name, true);
      if(vd) {
	return vd;
      }
    }
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
    return findType(name)->getDecl();
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

const std::vector<const Type*> ScopeCreator::getTypes() const {
  std::vector<const Type*> ret;
  for(const std::pair<const std::unique_ptr<Type>, AccessModifier>& t : ownedTypes) {
    ret.push_back(t.first.get());
  }
  return ret;
}

const FunctionDecl* ScopeCreator::findFunctionDecl(const std::string& name, bool external) const {
  // breadth first(ish)
  const std::vector<const FunctionDecl*> decls = getFunctionDecls();
  for(const FunctionDecl* fd : decls) {
    if(*fd->getName() == name) {
      return fd;
    }
  }
  if(this == owner) {
    // this is the module (top-level)
    if(external) {
      // this was call came from an outside module stop here
      return nullptr;
    }
    const Module* module = dynamic_cast<const Module*>(this);
    for(const llvm::StringMapEntry<std::unique_ptr<Module>>& ex : module->getExternals()) {
      const FunctionDecl* fd = ex.getValue()->findFunctionDecl(name, true);
      if(fd) {
	return fd;
      }
    }
    return nullptr;
  }
  else {
    return owner->findFunctionDecl(name);
  }
}

const Type* ScopeCreator::findType(const std::string& name, bool external) const {
  // breadth first(ish)
  const std::vector<const Type*> types = getTypes();
  for(const Type* t : types) {
    if(t->getName() == name) {
      return t;
    }
  }
  if(this == owner) {
    // this is the module (top-level)
    if(external) {
      // this was call came from an outside module stop here
      return nullptr;
    }
    const Module* module = dynamic_cast<const Module*>(this);
    for(const llvm::StringMapEntry<std::unique_ptr<Module>>& ex : module->getExternals()) {
      const Type* t = ex.getValue()->findType(name, true);
      if(t) {
	return t;
      }
    }
    return nullptr;
  }
  else {
    return owner->findType(name);
  }
}

const VarDecl* ScopeCreator::findVarDecl(const std::string& name, bool external) const {
  for(const VarDecl* var : ownedVars) {
    if(*var->getName() == name) {
      return var;
    }
  }
  if(owner == this) {
    // this is the module (top-level)
    if(external) {
      // this was call came from an outside module stop here
      return nullptr;
    }
    const Module* module = dynamic_cast<const Module*>(this);
    for(const llvm::StringMapEntry<std::unique_ptr<Module>>& ex : module->getExternals()) {
      const VarDecl* vd = ex.getValue()->findVarDecl(name, true);
      if(vd) {
	return vd;
      }
    }
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
    return findType(name)->getDecl();
  }
}
