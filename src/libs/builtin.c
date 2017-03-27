// C lib links
static int puts(const char* str);

// builtins
void print(const char* str) {
  puts(str);
}
