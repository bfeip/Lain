#include "type_decl.hpp"

void TypeDecl::setName(std::unique_ptr<std::string> str) {
  type->setName(*str.get());
  name = std::move(str);
  return;
}
