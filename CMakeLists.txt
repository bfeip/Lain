cmake_minimum_required(VERSION 3.4.3)
project(Lain)

set(LAIN_VERSION_MAJOR 0)
set(LAIN_VERSION_MINOR 0)

set(LLVM_DIR "./build/llvm/cmake/modules/CMakeFiles")
find_package(LLVM REQUIRED CONFIG)

include_directories(
  "./src/lexer/include"
  "./src/ast/include"
  "./src/parser"
  "./src/emitter"
  
  "./third_party/llvm/include"
  "./third_party/llvm/include/llvm"
  "./build/llvm/include"
  "./build/llvm/include/IR"
  "./build/llvm/lib/IR"
  ${LLVM_INCLUDE_DIRS}
  )

file(GLOB LEXER_SOURCES "./src/lexer/*.cpp")

file(GLOB AST_SOURCES "./src/ast/*.cpp")
file(GLOB AST_TESTS_SOURCES "./src/ast/tests/*.cpp")

file(GLOB PARSER_SOURCES "./src/parser/*.cpp")

file(GLOB EMITTER_SOURCES "./src/emitter/*.cpp")

add_definitions(-std=c++14 -g)

#add_executable(Lexer ${LEXER_SOURCES})
#add_executable(Parser ${PARSER_SOURCES} ${AST_SOURCES} "./src/lexer/lexer.cpp")
add_executable(Emitter ${EMITTER_SOURCES} ${AST_SOURCES} ${LLVM_IR_SOURCES}
  "./src/parser/parser.cpp" "./src/lexer/lexer.cpp")

llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(Emitter ${llvm_libs})
