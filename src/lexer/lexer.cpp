#include "lexer.hpp"
// <cctype> is needed

char lexer::pop() {
  return src.get();
}

char lexer::peek() {
  return src.peek();
}

std::string lexer::consume(bool strlit) {
  std::string ret = "";
  char c;
  if(strlit) {
    bool includeQuote = false;
    c = pop();
    while(c != EOF && (c != '\"' || includeQuote)) {
	ret += c;
	if(c == '\\' && peek() == '\"') {
	  includeQuote = true;
	}
	c = pop();
    }
    pop(); // remove end quote
  }
  else {
    c = peek(); // to stop from consuming whitespace
    while(c != EOF && isalpha(c)) {
      ret += c;
      pop(); // remove character that was added to ret
      c = peek();
    }
  }
  return ret;
}

Symbol next() {
  Symbol sym;
  char c = peek();
  switch(c) {
  case EOF:
    sym.setAll(TOK_EOF, line, col);
    break;
  case '.':
    sym.setAll(TOK_DOT, line, col);
    pop();
    break;
  case '=':
    sym.setPos(line, col);
    pop();
    if(peek() == '=') {
      sym.setToken(TOK_EQ);
      pop();
    }
    else {
      sym.setToken(TOK_ASSIGN);
    }
    break;
  case '>':
    sym.setPos(line, col);
    pop();
    if(peek() == '>') {
      pop();
      if(peek() == '=') {
	sym.setToken(TOK_RSH_ASSIGN);
	pop();
      }
      else {
	sym.setToken(TOK_RSH);
      }
    }
    else if(peek() == '=') {
      sym.setToken(TOK_GE);
      pop();
    }
    else {
      sym.setToken(TOK_GT);
    }
    break;
  case '<':
    sym.setPos(line, col);
    pop();
    if(peek() == '<') {
      pop();
      if(peek() == '=') {
	sym.setToken(TOK_LSH_ASSIGN);
	pop();
      }
      else {
	sym.setToken(TOK_LSH);
      }
    }
    else if(peek() == '=') {
      sym.setToken(TOK_LE);
      pop();
    }
    else {
      sym.setToken(TOK_LT);
    }
    break;
  case '&':
    sym.setPos(line, col);
    pop();
    if(peek() == '&') {
      sym.setToken(TOK_LOGI_AND);
      pop();
    }
    else if(peek() == '=') {
      sym.setToken(TOK_AND_ASSIGN);
      pop();
    }
    else {
      sym.setToken(TOK_AND);
    }
    break;
  case '|':
    sym.setPos(line, col);
    pop();
    if(peek() == '|') {
      sym.setToken(TOK_LOGI_OR);
      pop();
    }
    else if(peek() == '=') {
      sym.setToken(TOK_OR_ASSIGN);
      pop();
    }
    else {
      sym.setToken(TOK_OR);
    }
    break;
  case '^':
    sym.setPos(line, col);
    pop();
    if(peek() = '=') {
      sym.setToken(TOK_XOR_ASSIGN);
      pop();
    }
    else {
      sym.setToken(TOK_XOR);
    }
    break;
  case '-':
    sym.setPos(line, col);
    pop();
    if(peek() = '-') {
      sym.setToken(TOK_DEC);
      pop();
    }
    else if(peek() = '=') {
      sym.setToken(TOK_SUB_ASSIGN);
      pop();
    }
    else {
      sym.setToken(TOK_SUB);
    }
    break;
  case '+':
    sym.setPos(line, col);
    pop();
    if(peek() = '+') {
      sym.setToken(TOK_INC);
      pop();
    }
    else if(peek() = '=') {
      sym.setToken(TOK_ADD_ASSIGN);
      pop();
    }
    else {
      sym.setToken(TOK_ADD);
    }
    break;
  case '*':
    sym.setPos(line, col);
    pop();
    if(peek() == '=') {
      sym.setToken(TOK_MUL_ASSIGN);
      pop();
    }
    else {
      sym.setToken(TOK_MUL);
    }
    break;
  case '/':
    sym.setPos(line, col);
    pop();
    if(peek() == '=') {
      sym.setToken(TOK_DIV_ASSIGN);
      pop();
    }
    else if(peek() == '/') {
      sym.setToken(TOK_COMMENT);
      pop();
      std::string com = consumeLineComment();
      std::unique_ptr<SymbolData> dat(new SymbolData(com));
      sym.setSymbolData(dat);
    }
    else if(peek() == '*') {
      sym.setToken(TOK_COMMENT);
      pop();
      std::string com = consumeBlockComment();
      std::unique_ptr<SymbolData> dat(new SymbolData(com));
      sym.setSymbolData(dat);
    }
    else {
      sym.setToken(TOK_DIV);
    }
    break;
  case '%':
    sym.setPos(line, col);
    pop();
    if(peek() == '=') {
      sym.setToken(TOK_MOD_ASSIGN);
      pop();
    }
    else {
      sym.setToken(TOK_MOD);
    }
    break;
  case '!':
    sym.setPos(line, col);
    pop();
    if(peek() == '=') {
      sym.setToken(TOK_NE);
      pop();
    }
    else {
      sym.setToken(TOK_NOT);
      pop();
    }
  case ',':
    sym.setAll(TOK_COMMA, line, col);
    pop();
    break;
  case ':':
    sym.setAll(TOK_COLON, line, col);
    pop();
    break;
  case '(':
    sym.setAll(TOK_OPEN_PAREN, line, col);
    pop();
    break;
  case ')':
    sym.setAll(TOK_CLOSE_PAREN, line, col);
    pop();
    break;
  case '{':
    sym.setAll(TOK_OPEN_BRACKET, line, col);
    pop();
    break;
  case '}':
    sym.setAll(TOK_CLOSE_BRACKET, line, col);
    pop();
    break;
  case '\"':
    sym.setAll(TOK_STR_LIT, line, col);
    std::string str = consumeStrLit();
    std::unique_ptr<SymbolData> dat(new SymbolData(str));
    sym.setSymbolData(dat);
    break;
  case '\'':
    sym.setAll(TOK_CHAR_LIT, line, col);
    std::string chr= consumeCharLit();
    std::unique_ptr<SymbolData> dat(new SymbolData(chr));
    sym.setSymbolData(dat);
    break;
  }
  if(isspace(c)) {
    sym.setAll(TOK_WHITESPACE, line, col)
    pop();
    while(isspace(peek())) {
      pop();
    }
  }
  else if(isdigit(c)) {
    sym.setAll(TOK_NUM_LIT, line, col);
    std::string num = consumeNumLit();
    std::unique_ptr<SymbolData> dat(new SymbolData(num));
    sym.setSymbolData(dat);
  }
  else if(isalpha(c)) {
    sym.setPos(line, col);
    std::string id = consumeIdentifier();
    switch(id) {
    case "if":
      sym.setToken(TOK_IF);
      break;
    case "while":
      sym.setToken(TOK_WHILE);
      break;
    case "for":
      sym.setToken(TOK_FOR);
      break;
    case "switch":
      sym.setToken(TOK_SWITCH);
      break;
    case "case":
      sym.setToken(TOK_CASE);
      break;
    case "default":
      sym.setToken(TOK_DEFAULT);
      break;
    case "enum":
      sym.setToken(TOK_ENUM);
      break;
    case "class":
      sym.setToken(TOK_CLASS);
      break;
    case "return":
      sym.setToken(TOK_RETURN);
      break;
    default:
      sym.setToken(TOK_IDENTIFIER);
      std::unique_ptr<SymbolData> dat(new SymbolData(id));
      sym.setSymbolData(dat);
      break;
    }
  }
