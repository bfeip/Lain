#ifndef LEXER_HPP
#define LEXER_HPP

#include "lexer_shared.hpp"
#include "symbol.hpp"

class Lexer {
private:
  std::ifstream& src;
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
  Lexer(std::ifstream& src) : src(src), line(1), col(1) {}
  Symbol next();
};

#endif
