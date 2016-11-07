#ifndef PARSER_ERROR_HPP
#define PARSER_ERROR_HPP

#include <string>
#include "symbol.hpp"

class ParserError {
private:
  const std::string errstr;
  const std::string& filename;
  int line;
  int col;
public:
  ParserError(const std::string& es, const std::string& fn, const Symbol& sym)
    : errstr(es), filename(fn), line(sym.getLine()), col(sym.getCol()) {}
  void print();
};

#endif
