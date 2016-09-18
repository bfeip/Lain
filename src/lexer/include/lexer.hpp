#ifndef LEXER_HPP
#define LEXER_HPP

class Lexer {
private:
  std::ifstream& src;
  int line;
  int col;

  char pop();
  char peek();
  
  std::string consume(bool strlit); // if strlit, eat end quote too
public:
  lexer(std::ifstream& src) : src(src), line(1), col(1) {}
  Symbol next();
};

#endif
