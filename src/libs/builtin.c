// C lib links
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// builtins
/* later the print stuff will be conjoined into one function */
void print(const char* str) {
  puts(str);
}
    
char* prompt(const char* prompt) {
  unsigned size = 8;
  int i = 0;
  char* ret = malloc(size);
  char c = 0;
  printf("%s", prompt);
  while(1) {
    c = getc(stdin);
    if(c == '\n' || c == EOF) {
      return ret;
    }
    ret[i++] = c;
  }
}

unsigned _Strlen(char* str) {
  return strlen(str);
}
