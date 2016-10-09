#ifndef SCOPE_TREE_HPP
#define SCOPE_TREE_HPP

class ScopeTree {
private:
  struct Node {
    Type* type; // type == NULL <-> scope creator w/o type i.e. function
    
    Node* parent;
    std::vector<std::unique_ptr<Node>> children;
  };

  std::vector<Node> topTypes;
  Node* cur;
public:
  ScopeTree() : cur(nullptr), topTypes() {}

  void pushType(Type* t);
  void pushFunction();

  Type* findType(const std::string& name); // resolves namespaces i.e. this::is::a::type

  void move(const std::string& name); // move cur to type matching name
  void up() { cur = cur ? cur->parent : nullptr; }
};

#endif
