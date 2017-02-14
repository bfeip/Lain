#ifndef PARSER_HPP
#define PARSER_HPP

#include "parser_error.hpp"
#include "lexer.hpp"
#include "module.hpp"

class Parser {
private:
  std::string filename;
  Lexer lex;
  std::unique_ptr<Module> module;
  std::vector<ParserError> errors;
  std::vector<ParserError> warnings;

  void parseTop();
  
  void parseClassTop(Symbol& sym, ScopeCreator* owner);
  void parseClassInheritance(Symbol& sym, ClassDecl* cd);
  void parseClassBody(Symbol& sym, ClassDecl* cd);
  void parseClassVarInstatiation(Symbol& sym, ClassDecl* cd);

  void parseEnum(Symbol& sym, ScopeCreator* owner);
  
  void parseTypedef(Symbol& sym, ScopeCreator* owner);
  void parseIdDecl(Symbol& sym, ScopeCreator* owner);

  // function decl covers methods as well
  void parseFunctionDeclTop(Symbol& sym, FunctionDecl* fd);
  void parseFunctionDeclParams(Symbol& sym, FunctionDecl* fd);

  void parseCompoundStmt(Symbol& sym, CompoundStmt* cs);
  std::unique_ptr<Expr> parseExpr(Symbol& sym, std::string* id,
				  ScopeCreator* owner, Expr* parentExpr);
  void parseIfStmt(Symbol& sym, IfStmt* stmt);
  void parseElseStmt(Symbol& sym, ElseStmt* stmt);
  void parseWhileStmt(Symbol& sym, WhileStmt* stmt);
  void parseForStmt(Symbol& sym, ForStmt* stmt);
  void parseSwitchStmt(Symbol& sym, SwitchStmt* stmt);
  std::unique_ptr<AstNode> parseIdStmt(Symbol& sym, ScopeCreator* owner);
  
  void parseVarDeclStmt(Symbol& sym, VarDeclStmt* vds, std::string* id);

  void consumeObject(Symbol& sym); // consume until ';' or '}'
  void nextSignificantToken(Symbol& sym); // ignore whitepace, comments
  
  void storeError(const std::string& errstr, const Symbol& sym);
  void fatalError(const std::string& errstr, const Symbol& sym);
  void storeWarn(const std::string& errstr, const Symbol& sym);
public:
  Parser() = delete;
  Parser(const std::string& filename) : filename(filename), lex(filename),
					module(new Module(filename)) {}
  void parse();
  const Module* getModule() const { return module.get(); }
  std::unique_ptr<Module> stripModule();
};

#endif
