#ifndef DECL_HPP
#define DECL_HPP

#include "ast_shared.hpp"
#include "ast_node.hpp"
#include "scope_creator.hpp"

class Decl : virtual public AstNode {
protected:
  std::unique_ptr<std::string> name;
  
  ScopeCreator* owner;
public:
  Decl() = delete;
  Decl(ScopeCreator* sc) : owner(sc) {}
  virtual ~Decl() = default;
  
  std::string* getName() { return name.get(); }
  const std::string* getName() const { return name.get(); }
  void setName(std::unique_ptr<std::string>&& str) { name = std::move(str); }

  ScopeCreator* getOwner() { return owner; }
  const ScopeCreator* getOwner() const { return owner; }
  void setOwner(ScopeCreator* sc) { owner = sc; }
};

#endif
