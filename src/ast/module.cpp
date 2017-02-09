#include "module.hpp"

void Module::addGlobal(std::unique_ptr<VarDecl> global) {
  addOwnedVar(global.get());
  globals.emplace_back(std::move(global));
  return;
}
