#include "type_decl.hpp"

void TypeDecl::setName(std::unique_ptr<std::string> str) {
  name = std::move(str);
  type->setName(std::unique_ptr<std::string>(new std::string(*name.get())));
  return;
}
