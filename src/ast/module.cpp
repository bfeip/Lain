#include "module.hpp"

void Module::addGlobal(std::unique_ptr<VarDecl> global) {
  addOwnedVar(global.get());
  globals.push_back(std::move(global));
  return;
}
