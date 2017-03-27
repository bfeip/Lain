#include "emitter.hpp"

int main(int argc, const char** argv) {
  if(argc != 2) {
    std::cout << "Requires a filename";
    return -1;
  }

  Parser parser(argv[1]);
  parser.parse();
  std::unique_ptr<Module> module = parser.stripModule();

  llvm::LLVMContext context;
  Emitter emitter(context, std::move(module));
  emitter.emit();

  return 0;
}
  
