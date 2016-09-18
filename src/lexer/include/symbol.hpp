#ifndef SYMBOL_HPP
#define SYMBOL_HPP

class Symbol {
private:
  Token tok;
  std::unique_ptr<SymbolData> dat;
  int line;
  int col;
public:
  Symbol() = default;
  Symbol(Token tok, int line, int col) : tok(tok), dat(nullptr), line(line),
					 col(col) {}
  void setAll(Token t, int l, int c) { tok = t; line = l; col = c; }

  void setToken(Token t) { tok = t; }
  Token getToken() const { return tok; }
  
  void setSymbolData(std::unique_ptr<SymbolData> symdat) { dat = symdat; }
  const SymbolData& getSymbolData() const { return *dat; }

  void setPos(int l, int c) { line = l; col = c; }
  int getLine() const { return line; }
  int getCol() const { return col; }
};

#endif
