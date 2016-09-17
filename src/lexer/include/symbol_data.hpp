#ifndef SYMBOL_DATA_HPP
#define SYMBOL_DATA_HPP

class SymbolData {
  /* Stores symbol data (literals and identifiers) read from the TU (as a 
   * string). Every Symbol should have a unique_ptr to one of these */
private:
  std::unique_ptr<std::string> str;
public:
  Literal(const std::string& str) : str(new std::string(str)) {}
  
  const std::string& getAsString() { return *str; }
  int getAsInt() { return stoi(*str); }
  long getAsLong() { return stol(*str); }
  unsigned getAsUnsigned() { return stou(*str); }
  long long getAsLongLong() { return stoll(*str); }
  unsigned long long getAsUnsignedLongLong() { return stoull(*str); }
  float getAsFloat() { return stof(*str); }
  double getAsDouble() { return stod(*str); }
  long double getAsLongDouble() { return stold(*str); }
};

#endif
