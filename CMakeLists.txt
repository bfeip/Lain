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
  "./src/emitter"
  )

file(GLOB LEXER_SOURCES "./src/lexer/*.cpp")

file(GLOB AST_SOURCES "./src/ast/*.cpp")
file(GLOB AST_TESTS_SOURCES "./src/ast/tests/*.cpp")

file(GLOB PARSER_SOURCES "./src/parser/*.cpp")

file(GLOB EMITTER_SOURCES "./src/emitter/*.cpp")

add_definitions(-std=c++14 -g)

add_executable(Lexer ${LEXER_SOURCES})
add_executable(Parser ${PARSER_SOURCES} ${AST_SOURCES} "./src/lexer/lexer.cpp")
add_executable(Emitter ${EMITTER_SOURCES})
