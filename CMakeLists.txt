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
  "./src/parser/include"
  )

file(GLOB LEXER_SOURCES "./src/lexer/*.cpp")

add_definitions(-std=c++11)

add_executable(Lexer ${LEXER_SOURCES})
