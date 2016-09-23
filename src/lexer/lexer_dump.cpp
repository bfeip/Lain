#include "lexer.hpp"

int main(int argc, char** argv) {
  if(argc != 2) {
    cout << "Needs an input file\n";
    return 1;
  }
  std::ifstream in;
  in.open(argv[2]);
  if(!in) {
    cout << "Failed to open the input file for reading\n";
    return 2;
  }
  
  Lexer lex(in);
  Symbol sym = sym.next();
  const SymbolData* dat;
  while(sym.getToken() != TOK_EOF) {
    cout << sym.getLine() << ':' << sym.getCol() << '\t';
    cout << sym.getToken();
    if(dat = sym.getSymbolData()) {
      cout << '\t' << *dat.getAsString();
    }
    cout << endl;
  }

  return 0;
}
