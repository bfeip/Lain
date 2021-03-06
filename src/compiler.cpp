#include "emitter.hpp"
#include "Linker/Linker.h"
#include "IRReader/IRReader.h"
#include "Support/SourceMgr.h"
#include "Support/FileSystem.h"

std::vector<std::unique_ptr<llvm::Module>> getStdlibs(llvm::LLVMContext& context) {
  llvm::SMDiagnostic smd;
  std::vector<std::unique_ptr<llvm::Module>> ret;
  std::unique_ptr<llvm::Module> mod = llvm::parseIRFile("../src/libs/builtin.ll", smd, context);
  ret.push_back(std::move(mod));

  return ret;
}

void expandUsings(const Module* tu, llvm::StringMap<std::unique_ptr<Module>>& used) {
  const std::vector<std::string>& usings = tu->getUsings();
  for(const std::string& use : usings) {
    if(used.find(use) != used.end()) {
      Parser parser(use, used);
      parser.parse();
      std::unique_ptr<Module> usedModule = parser.stripModule();
      tu = usedModule.get();
      used.insert(std::make_pair(use, std::move(usedModule)));
      expandUsings(tu, used);
    }
  }

  return;
}

int main(int argc, char** argv) {
  if(argc < 2) {
    std::cout << "Requires at least one filename\n";
    return -2;
  }

  // parse all files and all usings
  llvm::StringMap<std::unique_ptr<Module>> translationUnits;
  Parser parser("../src/libs/builtin.lain", translationUnits);
  parser.parse();
  std::unique_ptr<Module> builtins = parser.stripModule();
  translationUnits.insert(std::make_pair("../src/libs/builtin.lain", std::move(builtins)));
  for(int i = 1; i < argc; i++) {
    if(translationUnits.find(argv[i]) != translationUnits.end()) {
      continue;
    }
    Parser parser(argv[i], translationUnits);
    parser.parse();
    std::unique_ptr<Module> module = parser.stripModule();
    Module* tu = module.get();
    translationUnits.insert(std::make_pair(argv[i], std::move(module)));
    expandUsings(tu, translationUnits);
  }

  llvm::LLVMContext context;
  llvm::Module final("a.ll", context);
  llvm::Linker linker(final);
  std::vector<std::unique_ptr<llvm::Module>> modules = getStdlibs(context);
  for(std::unique_ptr<llvm::Module>& module : modules) {
    linker.linkInModule(std::move(module));
  }
  for(llvm::StringMapEntry<std::unique_ptr<Module>>& tu : translationUnits) {
    Emitter emitter(context, tu.getValue().get());
    emitter.emit();
    std::unique_ptr<llvm::Module> module = emitter.stripModule();
    linker.linkInModule(std::move(module));
  }

  std::error_code ec;
  llvm::raw_fd_ostream out("./a.ll", ec, llvm::sys::fs::F_None);
  if(ec) {
    std::cout << "Could not open a.ll for output" << std::endl;
    std::cout << ec.message() << std::endl;
    return -2;
  }
  final.print(out, nullptr);
  
  return 0;
}
