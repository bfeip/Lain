#ifndef SYMBOL_HPP
#define SYMBOL_HPP

class Symbol {
private:
  Token tok;
  std::unique_ptr<SymbolData> dat;
  int line;
  int col;
public:
  Symbol(Token tok, int line, int col) : tok(tok), dat(nullptr), line(line),
					 col(col) {}

  Token getToken() { return tok; }
  
  void setSymbolData(std::unique_ptr<SymbolData> symdat) { dat = symdat; }
  const SymbolData& getSymbolData() { return *dat; }

  int getLine() { return line; }
  int getCol() { return col; }
};

#endif
