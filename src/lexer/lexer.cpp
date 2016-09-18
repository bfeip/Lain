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
    while(c != EOF && !isspace(c)) {
      ret += c;
      pop(); // remove character that was added to ret
      c = peek();
    }
  }
  return ret;
}
