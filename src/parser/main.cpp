#include "parser.hpp"

#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
  if(argc < 2) {
    std::cout << "Requires a filename\n";
    return -1;
  }

  // parse the file
  Parser parser(argv[1]);
  parser.parse(); // MAGIC

  const Module* module = parser.getModule();

  // add dump stuff here as I make tests

  return 0;
}
