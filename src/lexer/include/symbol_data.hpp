#ifndef SYMBOL_DATA_HPP
#define SYMBOL_DATA_HPP

#include "lexer_shared.hpp"

class SymbolData {
  /* Stores symbol data (literals and identifiers) read from the TU (as a 
   * string). Every Symbol should have a unique_ptr to one of these */
private:
  std::unique_ptr<std::string> str;
public:
  SymbolData(const std::string& str) : str(new std::string(str)) {}
  
  const std::string* getAsString() const { return str.get(); }
  int getAsInt() const { return stoi(*str); }
  long getAsLong() const { return stol(*str); }
  unsigned getAsUnsigned() const { return stoul(*str); }
  long long getAsLongLong() const { return stoll(*str); }
  unsigned long long getAsUnsignedLongLong() const { return stoull(*str); }
  float getAsFloat() const { return stof(*str); }
  double getAsDouble() const { return stod(*str); }
  long double getAsLongDouble() const { return stold(*str); }
};

#endif
