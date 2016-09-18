#ifndef LEXER_HPP
#define LEXER_HPP

class Lexer {
private:
  std::ifstream& src;

  char pop();
  char peek();
  
  std::string consume(bool strlit); // if strlit, eat end quote too
public:
  lexer(std::ifstream& src) : src(src) {}
  Symbol next();
};

#endif
