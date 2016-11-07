cmake_minimum_required(VERSION 2.6)
project(Lain)

set(LAIN_VERSION_MAJOR 0)
set(LAIN_VERSION_MINOR 0)

configure_file(
  "config/LainConfig.hpp.in"
  "config/LainConfig.hpp"
  )

include_directories(
  "./src/lexer/include"
  "./src/ast/include"
  "./src/parser"
  )

file(GLOB LEXER_SOURCES "./src/lexer/*.cpp")

file(GLOB AST_SOURCES "./src/ast/*.cpp")
file(GLOB AST_TESTS_SOURCES "./src/ast/tests/*.cpp")

file(GLOB PARSER_SOURCES "./src/parser/*.cpp")

add_definitions(-std=c++14)

add_executable(Lexer ${LEXER_SOURCES})
add_executable(Parser ${PARSER_SOURCES})
