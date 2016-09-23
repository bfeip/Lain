#ifndef TOKEN_HPP
#define TOKEN_HPP

enum Token {
  TOK_DOT,
  TOK_ASSIGN,
  TOK_RSH_ASSIGN,
  TOK_LSH_ASSIGN,
  TOK_AND_ASSIGN,
  TOK_OR_ASSIGN,
  TOK_XOR_ASSIGN,
  TOK_SUB_ASSIGN,
  TOK_ADD_ASSIGN,
  TOK_MUL_ASSIGN,
  TOK_DIV_ASSIGN,
  TOK_MOD_ASSIGN,

  TOK_LE,
  TOK_GE,
  TOK_RSH,
  TOK_LSH,
  TOK_LOGI_AND,
  TOK_LOGI_OR,
  TOK_INC,
  TOK_DEC,
  TOK_EQ,
  TOK_NEQ,
  TOK_NOT,
  TOK_AND,
  TOK_OR,
  TOK_SUB,
  TOK_ADD,
  TOK_MUL,
  TOK_DIV,
  TOK_MOD,
  TOK_XOR,
  TOK_LT,
  TOK_GT,

  TOK_WHITESPACE,
  TOK_COMMENT,
  
  TOK_COMMA,
  TOK_COLON,
  TOK_OPEN_PAREN,
  TOK_CLOSE_PAREN,
  TOK_OPEN_BRACKET,
  TOK_CLOSE_BRACKET,

  TOK_STR_LIT,
  TOK_CHAR_LIT,
  TOK_NUM_LIT,

  TOK_IDENTIFIER,

  TOK_IF,
  TOK_WHILE,
  TOK_FOR,
  TOK_SWITCH,
  TOK_CASE,
  TOK_DEFAULT,
  TOK_ENUM,
  TOK_CLASS,
  TOK_RETURN,

  TOK_EOF
};

#endif
