#include "include/ast.hpp"

void AST::proccessDecl(std::unique_ptr<Decl> decl) {
  if(!decl) {
    ast.storeError("attempt to add null decl");
    return;
  }
  
  if(TypeDecl* td = dynamic_cast<TypeDecl>(decl.get())) {
    Type* t = td->getType();
    if(!t) {
      ast.error("TypeDecl contained null type");
      return;
    }
    types.pushType(t);
  }

  if(FunctionDecl* fd = dynamic_cast<FunctionDecl>(decl.get())) {
    types.pushFunction();
  }

  if(curDecl != nullptr) {
    decl->setOwner(curDecl);
    curDecl->setOwned(std::move(decl));
  }
  else {
    topDecls.emplace_back(decl);
  }
  curDecl = decl;

  return;
}

void AST::endDecl() {
  curdecl = curdecl->getOwner();
}

Decl* AST::findDecl(const std::string& name) {
  if(!curDecl) {
    for(Decl* e : topDecls) {
      if(e->getName() == name) {
	return e;
      }
    }
  }
  
  if(name == curDecl->getName()) {
    return curDecl;
  }
  
  Decl* above = curDecl->getOwner();
  while(above) {
    if(above->getName == name) {
      return above;
    }
    above = above->getOwner();
  }
  
  if(FunctionDecl* fd = dynamic_cast<FunctionDecl>(curDecl)) {
    const std::vector<Decl*>& below = fd->getOwned();
    for(Decl*& e : below) {
      if(e->getName() == name) {
	return e;
      }
    }
  }
  
  if(ClassDecl* cd = dynamic_cast<ClassDecl>(curDecl)) {
    const std::vector<Decl*>& below = cd->getOwned();
    for(Decl* e : below) {
      if(e->getName() == name) {
	return e;
      }
    }
  }

  return nullptr;
}

Type* AST::findType(const std::string& name) {
  return types.findType(name);
}

Type* AST::addUnresolvedType(const std::string& name) {
  std::unique_ptr<Type> t(new Type(name));
  unresolvedTypes.emplace_back(t.get());
  types.pushType(std::move(t))
  return unresolvedTypes.last();
}
