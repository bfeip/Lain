#include "lexer.hpp"
#include <iostream>

int main(int argc, char** argv) {
  if(argc != 2) {
    std::cout << "Needs an input file\n";
    return 1;
  }
  std::ifstream in;
  in.open(argv[1]);
  if(!in) {
    std::cout << "Failed to open the input file for reading\n";
    return 2;
  }
  
  Lexer lex(in);
  Symbol sym = lex.next();
  const SymbolData* dat;
  while(sym.getToken() != TOK_EOF) {
    std::cout << sym.getLine() << ':' << sym.getCol() << '\t';
    std::cout << sym.getToken();
    if(dat = sym.getSymbolData()) {
      std::cout << '\t' << *dat->getAsString();
    }
    std::cout << std::endl;
    sym = lex.next();
  }

  return 0;
}
