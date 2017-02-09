#include "lexer.hpp"

char Lexer::pop() {
  char ret = src.get();
  if(ret == '\n') {
    line++;
    col = 1;
  }
  else {
    col++;
  }
  return ret;
}

char Lexer::peek() {
  return src.peek();
}

std::string Lexer::consumeLineComment() {
  std::string ret = "";
  while(peek() != EOF && peek() != '\n') {
    ret += pop();
  }
  if(peek() != EOF) {
    pop();
  }
  return ret;
}

std::string Lexer::consumeBlockComment() {
  std::string ret = "";
  while(peek() != EOF) {
    if(peek() == '*') {
      pop();
      if(peek() == '/') {
	return ret;
      }
      ret += '*';
    }
    else {
      ret += pop();
    }
  }
  return ret;
}

std::string Lexer::consumeStrLit() {
  std::string ret = "";
  while(peek() != EOF) {
    if(peek() == '\\') {
      ret += pop();
      if(peek() == '\"') {
	ret += '\"';
	pop();
      }
    }
    else if(peek() == '\"') {
      pop();
      return ret;
    }
    else {
      ret += pop();
    }
  }
  return ret;
}

std::string Lexer::consumeCharLit() {
  std::string ret = "";
  while(peek() != EOF) {
    if(peek() == '\\') {
      ret += pop();
      if(peek() == '\'') {
	ret += '\'';
	pop();
      }
    }
    else if(peek() == '\'') {
      pop();
      return ret;
    }
    else {
      ret += pop();
    }
  }
  return ret;
}

std::string Lexer::consumeNumLit() {
  std::string ret = "";
  char c;
  while((c = peek()) != EOF) {
    if(!isdigit(c) && c != '.') {
      return ret;
    }
    else {
      ret += pop();
    }
  }
  return ret;
}

std::string Lexer::consumeIdentifier() {
  std::string ret = "";
  char c;
  while((c = peek()) != EOF) {
    if(!isalnum(c)) {
      return ret;
    }
    else {
      ret += pop();
    }
  }
  return ret;
}

Symbol Lexer::next() {
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
    if(peek() == '^') {
      sym.setToken(TOK_LOGI_XOR);
      pop();
    }
    else if(peek() == '=') {
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
    if(peek() == '-') {
      sym.setToken(TOK_DEC);
      pop();
    }
    else if(peek() == '=') {
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
    if(peek() == '+') {
      sym.setToken(TOK_INC);
      pop();
    }
    else if(peek() == '=') {
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
      sym.setSymbolData(std::move(dat));
    }
    else if(peek() == '*') {
      sym.setToken(TOK_COMMENT);
      pop();
      std::string com = consumeBlockComment();
      std::unique_ptr<SymbolData> dat(new SymbolData(com));
      sym.setSymbolData(std::move(dat));
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
      sym.setToken(TOK_NEQ);
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
  case ';':
    sym.setAll(TOK_SEMICOLON, line, col);
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
  case '[':
    sym.setAll(TOK_OPEN_SQUARE_BRACKET, line, col);
    pop();
    break;
  case ']':
    sym.setAll(TOK_CLOSE_SQUARE_BRACKET, line, col);
    pop();
    break;
  case '\"':
    sym.setAll(TOK_STR_LIT, line, col);
    pop();
    {
      std::string str = consumeStrLit();
      std::unique_ptr<SymbolData> dat(new SymbolData(str));
      sym.setSymbolData(std::move(dat));
    }
    break;
  case '\'':
    sym.setAll(TOK_CHAR_LIT, line, col);
    pop();
    {
      std::string chr= consumeCharLit();
      std::unique_ptr<SymbolData> dat(new SymbolData(chr));
      sym.setSymbolData(std::move(dat));
    }
    break;
  default:
    if(isspace(c)) {
      sym.setAll(TOK_WHITESPACE, line, col);
      pop();
      while(isspace(peek())) {
	pop();
      }
    }
    else if(isdigit(c)) {
      sym.setAll(TOK_NUM_LIT, line, col);
      std::string num = consumeNumLit();
      std::unique_ptr<SymbolData> dat(new SymbolData(num));
      sym.setSymbolData(std::move(dat));
    }
    else if(isalpha(c)) {
      sym.setPos(line, col);
      std::string id = consumeIdentifier();
      if(id == "if") {
	sym.setToken(TOK_IF);
      }
      else if(id == "else") {
	sym.setToken(TOK_ELSE);
      }
      else if(id == "while") {
	sym.setToken(TOK_WHILE);
      }
      else if(id == "for") {
	sym.setToken(TOK_FOR);
      }
      else if(id == "switch") {
	sym.setToken(TOK_SWITCH);
      }
      else if(id == "case") {
	sym.setToken(TOK_CASE);
      }
      else if(id == "default") {
	sym.setToken(TOK_DEFAULT);
      }
      else if(id == "break") {
	sym.setToken(TOK_BREAK);
      }
      else if(id == "continue") {
	sym.setToken(TOK_CONTINUE);
      }
      else if(id == "enum") {
	sym.setToken(TOK_ENUM);
      }
      else if(id == "class") {
	sym.setToken(TOK_CLASS);
      }
      else if(id == "typedef") {
	sym.setToken(TOK_TYPEDEF);
      }
      else if(id == "public") {
	sym.setToken(TOK_PUBLIC);
      }
      else if(id == "protected") {
	sym.setToken(TOK_PROTECTED);
      }
      else if(id == "private") {
	sym.setToken(TOK_PRIVATE);
      }
      else if(id == "static") {
	sym.setToken(TOK_STATIC);
      }
      else if(id == "const") {
	sym.setToken(TOK_CONST);
      }
      else if(id == "return") {
	sym.setToken(TOK_RETURN);
      }
      else {
	sym.setToken(TOK_IDENTIFIER);
	std::unique_ptr<SymbolData> dat(new SymbolData(id));
	sym.setSymbolData(std::move(dat));
      }
    }
  }
  return sym;
}
