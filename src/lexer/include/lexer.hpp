#ifndef LEXER_HPP
#define LEXER_HPP

#include "lexer_shared.hpp"
#include "symbol.hpp"

class Lexer {
private:
  std::ifstream src;
  int line;
  int col;

  char pop();
  char peek();
  
  std::string consumeLineComment();
  std::string consumeBlockComment();
  std::string consumeStrLit();
  std::string consumeCharLit();
  std::string consumeNumLit();
  std::string consumeIdentifier();
public:
  Lexer(std::string filename) : src(filename), line(1), col(1) {
    if(!src) {
      // fuck
      std::cout << "File " << filename << " could not be opened for reading" << std::endl;
      exit(-2);
    }
  }
  Symbol next();
};

#endif
