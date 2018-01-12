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

file(GLOB LIBS_SOURCES "./src/libs/*.cpp")

file(GLOB LEXER_HEADERS "./src/lexer/include/*.hpp")
file(GLOB AST_HEADERS "./src/ast/include/*.hpp")
file(GLOB PARSER_HEADERS "./src/parser/*.hpp")

add_definitions(-std=c++14 -g -O0)

#add_executable(lexer ${LEXER_SOURCES})
#add_executable(parser ${PARSER_SOURCES} ${AST_SOURCES} "./src/lexer/lexer.cpp")
#add_executable(emitter ${EMITTER_SOURCES} ${AST_SOURCES} ${LIBS_SOURCES}
#  "./src/parser/parser.cpp" "./src/lexer/lexer.cpp")
add_executable(lain ${AST_SOURCES} ${LIBS_SOURCES} ${ALL_HEADERS}
  ${LEXER_HEADERS} ${AST_HEADERS} ${PARSER_HEADERS} "./src/lexer/lexer.cpp"
  "./src/parser/parser.cpp" "./src/emitter/emitter.cpp" "./src/compiler.cpp")

llvm_map_components_to_libnames(llvm_libs support core irreader linker analysis)
#target_link_libraries(emitter ${llvm_libs})
target_link_libraries(lain ${llvm_libs})
