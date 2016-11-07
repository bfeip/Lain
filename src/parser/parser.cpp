#include "parser.hpp"

void Parser::parseTop() {
  Symbol sym;
  nextSignificantToken(sym);
  while(sym.getToken() != TOK_EOF) {
    switch(sym.getToken()) {
    case TOK_CLASS:
      parseClassTop(sym, dynamic_cast<ScopeCreator*>(&module));
      break;
    case TOK_TYPEDEF:
      parseTypedef(sym, dynamic_cast<ScopeCreator*>(&module));
      break;
    case TOK_IDENTIFIER:
      parseIdDecl(sym, dynamic_cast<ScopeCreator*>(&module));
      break;
    default:
      storeError("WTF at top level", sym);
      consumeObject(sym);
      break;
    }
    nextSignificantToken(sym);
  } 
  return;
}

void Parser::parseClassTop(Symbol& sym, ScopeCreator* owner) {
  std::unique_ptr<ClassDecl> ast_classdecl(new ClassDecl(owner));
  ClassDecl* classdecl = ast_classdecl.get(); // messy, but don't want to use new
  bool anon = false;
  owner->addOwnedType(std::move(ast_classdecl), AM_VOID);

  nextSignificantToken(sym);
  if(sym.getToken() == TOK_IDENTIFIER) {
    // named class
    const std::string& classname = *sym.getSymbolData()->getAsString();
    if(const Decl* d = module.findDecl(classname)) {
      // decl of this type exists
      storeError("Name collision", sym);
      consumeObject(sym);
      return;
    }
    classdecl->setName(std::make_unique<std::string>(classname));
  }
  else {
    // anon class
    anon = true;
  }

  if(!anon) {
    nextSignificantToken(sym);
  }
  switch(sym.getToken()) {
  case TOK_EOF:
    fatalError("Unexpected EOF", sym);
    return;
  case TOK_SEMICOLON:
    // class forward decl
    if(anon) {
      storeWarn("Just the class keyword and a semicolon :/", sym);
    }
    return;
  case TOK_COLON:
    // inheritance
    parseClassInheritance(sym, classdecl);
    if(sym.getToken() == TOK_SEMICOLON) {
      // is the child of one or more classes but has no "unique" members
      return;
    }
    else if(sym.getToken() != TOK_OPEN_BRACKET) {
      storeError("WTF in class decl", sym);
    }
    // fall thru to class definition
  case TOK_OPEN_BRACKET:
    // class definition
    parseClassBody(sym, classdecl);
    break;
  default:
    storeError("WTF in class decl", sym);
    consumeObject(sym);
    return;
  }

  nextSignificantToken(sym);
  if(sym.getToken() != TOK_SEMICOLON) {
    // vars of this type being instantiated
    parseClassVarInstatiation(sym, classdecl);
  }
  else if(anon) {
    storeError("anon class with no vars / useless class", sym);
  }

  return;
}

void Parser::parseClassInheritance(Symbol& sym, ClassDecl* cd) {
  AccessModifier am;
  nextSignificantToken(sym);
  while(sym.getToken() != TOK_EOF) {
    // get access modifier or semicolon / open bracket
    switch(sym.getToken()) {
    case TOK_EOF:
      storeError("Unexpected EOF", sym);
      return;
    case TOK_OPEN_BRACKET:
    case TOK_SEMICOLON:
      return;
    case TOK_PUBLIC:
      am = AM_PUBLIC;
      break;
    case TOK_PROTECTED:
      am = AM_PROTECTED;
      break;
    case TOK_PRIVATE:
      am = AM_PRIVATE;
      break;
    default:
      storeError("WTF in class decl inheritance list", sym);
      consumeObject(sym);
      return;
    }

    // get inherited class name
    nextSignificantToken(sym);
    if(sym.getToken() != TOK_IDENTIFIER) {
      storeError("Not a typename", sym);
      consumeObject(sym);
      return;
    }

    // link this class to its parents and vice versa
    const std::string& inhername = *sym.getSymbolData()->getAsString();
    Type* inher = module.findType(inhername);
    cd->getType()->addParent(inher, am);
    inher->addChild(cd->getType(), am);

    // get comma or semicolon or open bracket
    nextSignificantToken(sym);
    switch(sym.getToken()) {
    case TOK_EOF:
      fatalError("Unexpected EOF", sym);
      return;
    case TOK_COMMA:
      break;
    case TOK_SEMICOLON:
    case TOK_OPEN_BRACKET:
      return;
    default:
      storeError("WTF in class decl inheritence list", sym);
      consumeObject(sym);
      return;
    }
  }
  
  return;
}
    
void Parser::parseClassBody(Symbol& sym, ClassDecl* cd) {
  AccessModifier am = AM_PUBLIC;
  while(sym.getToken() != TOK_CLOSE_BRACKET) {
    switch(sym.getToken()) {
    case TOK_EOF:
      storeError("Unexpected EOF", sym);
      return;
    case TOK_PUBLIC:
      am = AM_PUBLIC;
      nextSignificantToken(sym);
      if(sym.getToken() != TOK_COLON) {
	storeError("Expected colon after access modifier", sym);
	continue;
      }
      break;
    case TOK_PROTECTED:
      am = AM_PROTECTED;
      nextSignificantToken(sym);
      if(sym.getToken() != TOK_COLON) {
	storeError("Expected colon after access modifier", sym);
	continue;
      }
      break;
    case TOK_PRIVATE:
      am = AM_PRIVATE;
      nextSignificantToken(sym);
      if(sym.getToken() != TOK_COLON) {
	storeError("Expected colon after access modifier", sym);
	continue;
      }
      break;
    case TOK_IDENTIFIER:
      parseIdDecl(sym, dynamic_cast<ScopeCreator*>(cd));
      break;
    }
    nextSignificantToken(sym);
  }

  return;
}

void Parser::parseClassVarInstatiation(Symbol& sym, ClassDecl* cd) {
  ScopeCreator* owner = cd->getOwner();
  std::unique_ptr<VarDeclStmt> vds(new VarDeclStmt(owner));
  vds->setType(std::make_shared<QualType>(cd->getType()));
  
  if(dynamic_cast<Module*>(owner)) {
    // global variable instantation
    parseVarDeclStmt(sym, vds.get(), nullptr);
    std::vector<std::unique_ptr<VarDecl>>& vars = vds->stripVars();
    for(std::unique_ptr<VarDecl>& e : vars) {
      module.addGlobal(std::move(e));
    }
    return;
  }
  
  if(ClassDecl* owningClass = dynamic_cast<ClassDecl*>(owner)) {
    // member variable instantation
    parseVarDeclStmt(sym, vds.get(), nullptr);
    std::vector<std::unique_ptr<VarDecl>>& vars = vds->stripVars();
    for(std::unique_ptr<VarDecl>& e : vars) {
      owningClass->addMember(std::move(e), AM_VOID); // TODO add AM support
    }
    return;
  }

  if(CompoundStmt* owningStmt = dynamic_cast<CompoundStmt*>(owner)) {
    // instantiation in function
    parseVarDeclStmt(sym, vds.get(), nullptr);
    owningStmt->addStmt(std::move(vds));
    return;
  }
    
  fatalError("Missing case in parser", sym);
  return;
}

void Parser::parseTypedef(Symbol& sym, ScopeCreator* owner) {
  std::unique_ptr<TypeDefDecl> ast_tdd(new TypeDefDecl(owner));
  TypeDefDecl* tdd = ast_tdd.get();
  owner->addOwnedType(std::move(ast_tdd), AM_VOID);

  nextSignificantToken(sym);
  if(sym.getToken() != TOK_IDENTIFIER) {
    storeError("WTF in typedef", sym);
    consumeObject(sym);
    return;
  }
  const std::string& typeName = *sym.getSymbolData()->getAsString();
  tdd->setName(std::make_unique<std::string>(typeName));

  nextSignificantToken(sym);
  if(sym.getToken() != TOK_IDENTIFIER) {
    storeError("WTF in typedef", sym);
    consumeObject(sym);
    return;
  }
  tdd->setDefinition(*sym.getSymbolData()->getAsString());

  nextSignificantToken(sym);
  if(sym.getToken() != TOK_SEMICOLON) {
    storeError("WTF in typedef", sym);
    consumeObject(sym);
    return;
  }

  return;
}

void Parser::parseIdDecl(Symbol& sym, ScopeCreator* owner) {
  bool qtStatic = false;
  bool qtConst = false;
  while(sym.getToken() != TOK_IDENTIFIER) {
    // get type qualifiers
    switch(sym.getToken()) {
    case TOK_STATIC:
      qtStatic = true;
      break;
    case TOK_CONST:
      qtConst = true;
      break;
    default:
      storeError("WTF in qual type", sym);
      consumeObject(sym);
      return;
    }
    nextSignificantToken(sym);
  }

  // get name of base type and construct QualType
  const std::string& typeName = *sym.getSymbolData()->getAsString();
  Type* t = module.findType(typeName);
  std::unique_ptr<QualType> qt(new QualType(t));
  qt->setStatic(qtStatic);
  qt->setConst(qtConst);

  // get name of decl
  nextSignificantToken(sym);
  if(sym.getToken() != TOK_IDENTIFIER) {
    storeError("WTF in id decl", sym);
    consumeObject(sym);
    return;
  }
  const std::string& name = *sym.getSymbolData()->getAsString();

  // determine what kind of decl (function or var)
  nextSignificantToken(sym);
  
  switch(sym.getToken()) {
  case TOK_OPEN_PAREN:
    // Function or method decl
    {
      std::unique_ptr<FunctionDecl> ast_fd(new FunctionDecl(owner));
      FunctionDecl* fd = ast_fd.get();
      owner->addOwnedFunction(std::move(ast_fd), AM_VOID); // TODO AM stuff
      fd->setName(std::make_unique<std::string>(name));
      fd->setReturnType(std::move(qt));
      parseFunctionDeclTop(sym, fd);
      return;
    }
  case TOK_COMMA:
  case TOK_ASSIGN:
    // global var decl stmt or member decl stmt
    {
      std::unique_ptr<VarDeclStmt> vds(new VarDeclStmt(owner));
      std::string typeName = name;
      parseVarDeclStmt(sym, vds.get(), &typeName);
      if(dynamic_cast<Module*>(owner)) {
	// gloabal var decl stmt
	std::vector<std::unique_ptr<VarDecl>>& vars = vds->stripVars();
	for(std::unique_ptr<VarDecl>& e : vars) {
	  module.addGlobal(std::move(e));
	}
      }
      else {
	// member var decl stmt
	ClassDecl* cd = dynamic_cast<ClassDecl*>(owner);
   	std::vector<std::unique_ptr<VarDecl>>& vars = vds->stripVars();
	for(std::unique_ptr<VarDecl>& e : vars) {
	  cd->addMember(std::move(e), AM_VOID); // TODO make ams work here
	}
      }
    }
    return;
  default:
    storeError("WTF in id decl", sym);
    consumeObject(sym);
    return;
  }

  return;
}

void Parser::parseFunctionDeclTop(Symbol& sym, FunctionDecl* fd) {
  if(sym.getToken() != TOK_OPEN_PAREN) {
    fatalError("parseFunctionDeclTop() not passed TOK_OPEN_PAREN", sym);
    return;
  }
  parseFunctionDeclParams(sym, fd);

  switch(sym.getToken()) {
  case TOK_EOF:
    fatalError("Unexpected EOF", sym);
    return;
  case TOK_SEMICOLON:
    // function prototype
    storeError("Function prototypes are not allowed", sym);
    consumeObject(sym);
    return;
  case TOK_OPEN_BRACKET:
    // function definition
    {
      std::unique_ptr<CompoundStmt> fn_body(new CompoundStmt(fd));
      CompoundStmt* body = fn_body.get();
      fd->setBody(std::move(fn_body));
      parseCompoundStmt(sym, body);
      return;
    }
  default:
    storeError("WTF in Function decl", sym);
    consumeObject(sym);
    return;
  }
}
    
void Parser::parseFunctionDeclParams(Symbol& sym, FunctionDecl* fd) {
  nextSignificantToken(sym);
  bool qtStatic = false;
  bool qtConst = false;
  while(sym.getToken() != TOK_CLOSE_PAREN) {
    while(sym.getToken() != TOK_IDENTIFIER) {
      // get type qualifiers
      switch(sym.getToken()) {
      case TOK_STATIC:
	qtStatic = true;
	break;
      case TOK_CONST:
	qtConst = true;
	break;
      default:
	storeError("WTF in qual type", sym);
	consumeObject(sym);
	return;
      }
      nextSignificantToken(sym);
    }
    
    // get name of base type
    const std::string& typeName = *sym.getSymbolData()->getAsString();
    Type* t = module.findType(typeName);
    std::shared_ptr<QualType> qt(new QualType(t));
    qt->setConst(qtConst);
    qt->setStatic(qtStatic);

    // in lain parameter names are mandatory
    nextSignificantToken(sym);
    if(sym.getToken() != TOK_IDENTIFIER) {
      storeError("Not a vaild parameter name", sym);
      consumeObject(sym);
      return;
    }

    // add param to function decl
    const std::string paramName = *sym.getSymbolData()->getAsString();
    std::unique_ptr<VarDecl> param(new VarDecl(nullptr, std::move(qt))); // nullptr could be problem
    param->setName(std::make_unique<std::string>(paramName));
    fd->addParam(std::move(param));

    nextSignificantToken(sym);
    switch(sym.getToken()) {
    case TOK_EOF:
      fatalError("Unexpected EOF", sym);
      return;
    case TOK_COMMA:
      nextSignificantToken(sym);
      break;
    case TOK_EQ:
      storeError("Default params not yet supported", sym);
      consumeObject(sym);
      return;
    default:
      storeError("WTF in param list", sym);
      consumeObject(sym);
      return;
    }
  }

  return;
}

void Parser::parseCompoundStmt(Symbol& sym, CompoundStmt* cs) {      
  nextSignificantToken(sym);
  while(sym.getToken() != TOK_CLOSE_BRACKET) {
    switch(sym.getToken()) {
    case TOK_EOF:
      fatalError("Unexpected EOF", sym);
      return;

    case TOK_SEMICOLON:
      // null stmt
      {
	std::unique_ptr<NullStmt> stmt(new NullStmt(cs));
	cs->addStmt(std::move(stmt));
      }
      break;

    case TOK_OPEN_BRACKET:
      // compound stmt
      {
	std::unique_ptr<CompoundStmt> stmt(new CompoundStmt(cs));
	cs->addStmt(std::move(stmt));
	parseCompoundStmt(sym, stmt.get());
      }
      break;

    case TOK_CLASS:
      // embeded class decl
      parseClassTop(sym, cs);
      break;
      
    case TOK_ADD:
    case TOK_SUB:
    case TOK_INC:
    case TOK_DEC:
    case TOK_NOT:
      // unary operator
      {
	std::unique_ptr<Expr> stmt = parseExpr(sym, nullptr, cs);
	cs->addStmt(std::move(stmt));
      }
      break;

    case TOK_IF:
      // if stmt
      {
      	std::unique_ptr<IfStmt> stmt(new IfStmt(cs));
	cs->addStmt(std::move(stmt));
	parseIfStmt(sym, stmt.get());
      }
      break;

    case TOK_ELSE:
      {
	if(!dynamic_cast<IfStmt*>(cs->getStmts().back())) {
	  storeError("Else statement without if", sym);
	  break;
	}
	std::unique_ptr<ElseStmt> stmt(new ElseStmt(cs));
	cs->addStmt(std::move(stmt));
	stmt->setIf(dynamic_cast<IfStmt*>(cs->getStmts().back()));
	parseElseStmt(sym, stmt.get());
      }
      break;

    case TOK_WHILE:
      {
	std::unique_ptr<WhileStmt> stmt(new WhileStmt(cs));
	cs->addStmt(std::move(stmt));
	parseWhileStmt(sym, stmt.get());
      }
      break;

    case TOK_FOR:
      {
	std::unique_ptr<ForStmt> stmt(new ForStmt(cs));
	cs->addStmt(std::move(stmt));
	parseForStmt(sym, stmt.get());
      }
      break;

    case TOK_SWITCH:
      {
	std::unique_ptr<SwitchStmt> stmt(new SwitchStmt(cs));
	cs->addStmt(std::move(stmt));
	parseSwitchStmt(sym, stmt.get());
      }
      break;

    case TOK_BREAK:
      {
	std::unique_ptr<BreakStmt> stmt(new BreakStmt(cs));
	cs->addStmt(std::move(stmt));
	nextSignificantToken(sym);
	if(sym.getToken() != TOK_SEMICOLON) {
	  storeError("WTF after break stmt", sym);
	  consumeObject(sym);
	}
      }
      break;

    case TOK_CONTINUE:
      {
	std::unique_ptr<ContinueStmt> stmt(new ContinueStmt(cs));
	cs->addStmt(std::move(stmt));
	nextSignificantToken(sym);
	if(sym.getToken() != TOK_SEMICOLON) {
	  storeError("WTF after continue stmt", sym);
	  consumeObject(sym);
	}
      }
      break;

    case TOK_RETURN:
      {
	std::unique_ptr<ReturnStmt> ast_stmt(new ReturnStmt(cs));
	ReturnStmt* stmt = ast_stmt.get();
	cs->addStmt(std::move(ast_stmt));
	nextSignificantToken(sym);
	if(sym.getToken() == TOK_IDENTIFIER ||
	   sym.getToken() == TOK_NUM_LIT ||
	   sym.getToken() == TOK_STR_LIT ||
	   sym.getToken() == TOK_CHAR_LIT) {
	  // TODO change this to get an expr instead
	  stmt->setValue(parseExpr(sym, nullptr, stmt));
	}
      }
      break;

    case TOK_ENUM:
      parseEnum(sym, cs);
      break;

    case TOK_IDENTIFIER:
      {
	std::unique_ptr<AstNode> node(parseIdStmt(sym, cs));
	if(dynamic_cast<FunctionDecl*>(node.get())) {
	  std::unique_ptr<FunctionDecl> ast_fd(dynamic_cast<FunctionDecl*>(node.release()));
	  cs->addOwnedFunction(std::move(ast_fd), AM_VOID);
	  break;
	}
	if(dynamic_cast<Stmt*>(node.get())) {
	  std::unique_ptr<Stmt> ast_stmt(dynamic_cast<Stmt*>(node.release()));
	  cs->addStmt(std::move(ast_stmt));
	  break;
	}
	storeError("Unknown ID stmt", sym);
	consumeObject(sym);
	return;
      }
    default:
      storeError("WTF in compound stmt", sym);
      consumeObject(sym);
      break;
    }
    nextSignificantToken(sym);
  }

  return;
}

std::unique_ptr<Expr> Parser::parseExpr(Symbol& sym, std::string* id,
					Stmt* owner) {
  // I'm so so so sorry
  std::string leftStr = "";
  if(id) {
    if(owner) {
      // left courld be a variable expression or function call
      leftStr = *id;
    }
    else {
      // this shouldn't be possible... I think
      storeError("I honestly don't know if this is okay", sym);
      return std::move(std::unique_ptr<Expr>(nullptr));
    }
  }
  
  if(sym.getToken() == TOK_IDENTIFIER) {
    leftStr = *sym.getSymbolData()->getAsString();
    nextSignificantToken(sym);
  }
  
  if(leftStr != "") {
    // there is a left operand
    switch(sym.getToken()) {
    case TOK_EOF:
      fatalError("Unexpected EOF", sym);
      return std::move(std::unique_ptr<Expr>(nullptr));
    case TOK_SEMICOLON:
    case TOK_CLOSE_PAREN:
    case TOK_CLOSE_SQUARE_BRACKET:
      // end of expr
      {
	VarDecl* var = owner->findVar(leftStr);
	std::unique_ptr<VarInstanceExpr> ret(new VarInstanceExpr(owner, var));
	return std::move(ret);
      }
    case TOK_OPEN_PAREN:
      // left is the name of a function, this is a call
      {
	FunctionDecl* func = module.findFunctionDecl(leftStr);
	// TODO what if this func doesnt exist yet
	std::unique_ptr<FunctionCallExpr> ret(new FunctionCallExpr(owner, func));
	while(sym.getToken() != TOK_CLOSE_PAREN) {
	  // get args
	  ret->addArg(parseExpr(sym, nullptr, owner));
	}
	nextSignificantToken(sym);
	return std::move(ret);
      }
    case TOK_INC:
      // post inc
      {
	nextSignificantToken(sym);
	VarDecl* var = owner->findVar(leftStr);
	std::unique_ptr<VarInstanceExpr> operand(new VarInstanceExpr(owner, var));
	return std::make_unique<UnaryOperationExpr>(owner, std::move(operand), OP_POST_INC);
      }
    case TOK_DEC:
      // post dec
      {
	nextSignificantToken(sym);
	VarDecl* var = owner->findVar(leftStr);
	std::unique_ptr<VarInstanceExpr> operand(new VarInstanceExpr(owner, var));
	return std::make_unique<UnaryOperationExpr>(owner, std::move(operand), OP_POST_DEC);
      }
    case TOK_ADD:
      {
	nextSignificantToken(sym);
	VarDecl* var = owner->findVar(leftStr);
	std::unique_ptr<VarInstanceExpr> lOperand(new VarInstanceExpr(owner, var));
	std::unique_ptr<Expr> rOperand(std::move(parseExpr(sym, nullptr, owner)));
	return std::make_unique<BinaryOperationExpr>(owner, std::move(lOperand),
						     std::move(rOperand), OP_ADD);
      }
    case TOK_SUB:
      {
	nextSignificantToken(sym);
	VarDecl* var = owner->findVar(leftStr);
	std::unique_ptr<VarInstanceExpr> lOperand(new VarInstanceExpr(owner, var));
	std::unique_ptr<Expr> rOperand(std::move(parseExpr(sym, nullptr, owner)));
	return std::make_unique<BinaryOperationExpr>(owner, std::move(lOperand),
						     std::move(rOperand), OP_SUB);
      }
    case TOK_MUL:
      {
	nextSignificantToken(sym);
     	VarDecl* var = owner->findVar(leftStr);
	std::unique_ptr<VarInstanceExpr> lOperand(new VarInstanceExpr(owner, var));
	std::unique_ptr<Expr> rOperand(std::move(parseExpr(sym, nullptr, owner)));
	return std::make_unique<BinaryOperationExpr>(owner, std::move(lOperand),
						     std::move(rOperand), OP_MUL);
      }
    case TOK_DIV:
      {
	nextSignificantToken(sym);
	VarDecl* var = owner->findVar(leftStr);
	std::unique_ptr<VarInstanceExpr> lOperand(new VarInstanceExpr(owner, var));
	std::unique_ptr<Expr> rOperand(std::move(parseExpr(sym, nullptr, owner)));
	return std::make_unique<BinaryOperationExpr>(owner, std::move(lOperand),
						     std::move(rOperand), OP_DIV);
      }
    case TOK_MOD:
      {
	nextSignificantToken(sym);
	VarDecl* var = owner->findVar(leftStr);
	std::unique_ptr<VarInstanceExpr> lOperand(new VarInstanceExpr(owner, var));
	std::unique_ptr<Expr> rOperand(std::move(parseExpr(sym, nullptr, owner)));
	return std::make_unique<BinaryOperationExpr>(owner, std::move(lOperand),
						     std::move(rOperand), OP_ADD);
      }
    default:
      storeError("Unknown operation", sym);
      return std::unique_ptr<Expr>(nullptr);
    }
  }

  else {
    // there is no left operand, nor is there hope
    switch(sym.getToken()) {
    case TOK_OPEN_PAREN:
      // grouped expr
      {
	nextSignificantToken(sym);
	std::unique_ptr<Expr> body(std::move(parseExpr(sym, nullptr, owner)));
	return std::make_unique<GroupedExpr>(owner, std::move(body));
      }
    case TOK_INC:
      // pre inc
      {
	nextSignificantToken(sym);
	VarDecl* var = owner->findVar(leftStr);
	std::unique_ptr<Expr> operand(std::move(parseExpr(sym, nullptr, owner)));
	return std::make_unique<UnaryOperationExpr>(owner, std::move(operand), OP_PRE_INC);
      }
    case TOK_DEC:
      // pre dec
      {
	nextSignificantToken(sym);
	VarDecl* var = owner->findVar(leftStr);
      	std::unique_ptr<Expr> operand(std::move(parseExpr(sym, nullptr, owner)));
	return std::make_unique<UnaryOperationExpr>(owner, std::move(operand), OP_POST_INC);
      }
    case TOK_ADD:
      // take absolute value of operand
      {
	nextSignificantToken(sym);
	VarDecl* var = owner->findVar(leftStr);
      	std::unique_ptr<Expr> operand(std::move(parseExpr(sym, nullptr, owner)));
	return std::make_unique<UnaryOperationExpr>(owner, std::move(operand), OP_ABS);
      }
    case TOK_SUB:
      // negate operand
      {
	nextSignificantToken(sym);
	VarDecl* var = owner->findVar(leftStr);
      	std::unique_ptr<Expr> operand(std::move(parseExpr(sym, nullptr, owner)));
	return std::make_unique<UnaryOperationExpr>(owner, std::move(operand), OP_NEG);
      }
    default:
      storeError("Unknown operation", sym);
      return std::unique_ptr<Expr>(nullptr);
    }
  }

  return std::unique_ptr<Expr>(nullptr);
}
  

void Parser::parseIfStmt(Symbol& sym, IfStmt* stmt) {
  nextSignificantToken(sym);
  if(sym.getToken() != TOK_OPEN_PAREN) {
    storeError("If statement needs condition", sym);
    consumeObject(sym);
    return;
  }

  // get condition
  nextSignificantToken(sym);
  std::unique_ptr<Expr> if_expr(parseExpr(sym, nullptr, stmt));
  stmt->setCondition(std::move(if_expr));

  if(sym.getToken() != TOK_CLOSE_PAREN) {
    storeError("Something bad happened", sym);
    consumeObject(sym);
    return;
  }

  // get body
  // for now only compound statements can be bodies :(
  nextSignificantToken(sym);
  if(sym.getToken() != TOK_OPEN_BRACKET) {
    storeError("Only compound statements can be if bodies right now :(", sym);
    consumeObject(sym);
    return;
  }
  std::unique_ptr<CompoundStmt> if_cs(new CompoundStmt(stmt));
  CompoundStmt* cs = if_cs.get();
  stmt->setBody(std::move(if_cs));
  parseCompoundStmt(sym, cs);

  nextSignificantToken(sym);
  return;
}

void Parser::parseElseStmt(Symbol& sym, ElseStmt* stmt) {
  nextSignificantToken(sym);
  switch(sym.getToken()) {
  case TOK_EOF:
    fatalError("Unexpected EOF", sym);
    return;
  case TOK_IF:
    {
      std::unique_ptr<IfStmt> else_ifstmt(new IfStmt(stmt));
      IfStmt* ifstmt = else_ifstmt.get();
      stmt->setBody(std::move(else_ifstmt));
      parseIfStmt(sym, ifstmt);
    }
    break;
  case TOK_OPEN_BRACKET:
    {
      std::unique_ptr<CompoundStmt> else_cs(new CompoundStmt(stmt));
      CompoundStmt* cs = else_cs.get();
      stmt->setBody(std::move(else_cs));
      parseCompoundStmt(sym, cs);
    }
    break;
  default:
    storeError("Only blocks or ifs can be else bodies right now", sym);
    consumeObject(sym);
    return;
  }

  nextSignificantToken(sym);
  return;
}

void Parser::parseWhileStmt(Symbol& sym, WhileStmt* stmt) {
  nextSignificantToken(sym);
  if(sym.getToken() != TOK_OPEN_PAREN) {
    storeError("While statement needs condition", sym);
    consumeObject(sym);
    return;
  }

  // get condition
  nextSignificantToken(sym);
  std::unique_ptr<Expr> while_expr(parseExpr(sym, nullptr, stmt));
  stmt->setCondition(std::move(while_expr));

  if(sym.getToken() != TOK_CLOSE_PAREN) {
    storeError("Something bad happened", sym);
    consumeObject(sym);
    return;
  }

  // get body
  // for now only compound statements can be bodies :(
  nextSignificantToken(sym);
  if(sym.getToken() != TOK_OPEN_BRACKET) {
    storeError("Only compound statements can be if bodies right now :(", sym);
    consumeObject(sym);
    return;
  }
  std::unique_ptr<CompoundStmt> while_cs(new CompoundStmt(stmt));
  CompoundStmt* cs = while_cs.get();
  stmt->setBody(std::move(while_cs));
  parseCompoundStmt(sym, cs);

  nextSignificantToken(sym);
  return;
}

void Parser::parseForStmt(Symbol& sym, ForStmt* stmt) {
  nextSignificantToken(sym);
  if(sym.getToken() != TOK_OPEN_PAREN) {
    storeError("For statement needs condition", sym);
    consumeObject(sym);
    return;
  }

  // start
  nextSignificantToken(sym);
  switch(sym.getToken()) {
  case TOK_EOF:
    fatalError("Unexpected EOF", sym);
    return;
  case TOK_SEMICOLON:
    // move on to stop
    break;
  case TOK_IDENTIFIER:
    parseIdStmt(sym, stmt);
    break;
  default:
    storeError("WTF in for statement start", sym);
    consumeObject(sym);
    return;
  }

  // stop
  nextSignificantToken(sym);
  switch(sym.getToken()) {
  case TOK_EOF:
    fatalError("Unexpected EOF", sym);
    return;
  case TOK_SEMICOLON:
    storeError("For statment needs stop condition", sym);
    consumeObject(sym);
    return;
  default:
    std::unique_ptr<Expr> for_stopExpr(parseExpr(sym, nullptr, stmt));
    stmt->setStop(std::move(for_stopExpr));
  }

  // step
  nextSignificantToken(sym);
  switch(sym.getToken()) {
  case TOK_EOF:
    fatalError("Unexpected EOF", sym);
    return;
  case TOK_SEMICOLON:
    break;
  default:
    std::unique_ptr<Expr> for_stepExpr(parseExpr(sym, nullptr, stmt));
    stmt->setStep(std::move(for_stepExpr));
  }
  
  if(sym.getToken() != TOK_CLOSE_PAREN) {
    storeError("Expected close paren", sym);
  }

  nextSignificantToken(sym);
  if(sym.getToken() != TOK_OPEN_BRACKET) {
    storeError("Only compound statements can be bodies right now :(", sym);
    consumeObject(sym);
    return;
  }

  std::unique_ptr<CompoundStmt> for_body(new CompoundStmt(stmt));
  CompoundStmt* body = for_body.get();
  stmt->setBody(std::move(for_body));
  parseCompoundStmt(sym, body);

  return;
}

void Parser::parseSwitchStmt(Symbol& sym, SwitchStmt* stmt) {
  nextSignificantToken(sym);
  if(sym.getToken() != TOK_OPEN_PAREN) {
    storeError("Switch needs condition", sym);
    consumeObject(sym);
    return;
  }

  nextSignificantToken(sym);
  std::unique_ptr<Expr> switch_expr(parseExpr(sym, nullptr, stmt));
  stmt->setCondition(std::move(switch_expr));

  if(sym.getToken() != TOK_CLOSE_PAREN) {
    storeError("Expected close paren", sym);
  }

  nextSignificantToken(sym);
  if(sym.getToken() != TOK_OPEN_BRACKET) {
    storeError("Switch must be followed by statement block", sym);
    consumeObject(sym);
    return;
  }

  std::unique_ptr<CompoundStmt> switch_body(new CompoundStmt(stmt));
  CompoundStmt* body = switch_body.get();
  stmt->setBody(std::move(switch_body));
  parseCompoundStmt(sym, body);

  return;
}

std::unique_ptr<AstNode> Parser::parseIdStmt(Symbol& sym, Stmt* owner) {
  bool qtStatic = false;
  bool qtConst = false;
  if(sym.getToken() != TOK_IDENTIFIER) {
    fatalError("parseIdStmt not passed identifier", sym);
    return std::unique_ptr<Stmt>(nullptr);
  }
  std::string first = *sym.getSymbolData()->getAsString();

  nextSignificantToken(sym);
  switch(sym.getToken()) {
  case TOK_EOF:
    fatalError("Unexpected EOF", sym);
    return std::unique_ptr<Stmt>(nullptr);
   
  case TOK_LE:
  case TOK_GE:
  case TOK_RSH:
  case TOK_LSH:
  case TOK_LOGI_AND:
  case TOK_LOGI_OR:
  case TOK_INC:
  case TOK_DEC:
  case TOK_EQ:
  case TOK_NEQ:
  case TOK_AND:
  case TOK_OR:
  case TOK_SUB:
  case TOK_ADD:
  case TOK_MUL:
  case TOK_DIV:
  case TOK_MOD:
  case TOK_XOR:
  case TOK_LT:
  case TOK_GT:
    
  case TOK_OPEN_PAREN:
    
  case TOK_DOT:
  case TOK_ASSIGN:
  case TOK_RSH_ASSIGN:
  case TOK_LSH_ASSIGN:
  case TOK_AND_ASSIGN:
  case TOK_OR_ASSIGN:
  case TOK_XOR_ASSIGN:
  case TOK_SUB_ASSIGN:
  case TOK_ADD_ASSIGN:
  case TOK_MUL_ASSIGN:
  case TOK_DIV_ASSIGN:
  case TOK_MOD_ASSIGN:
    // any of the above make this an expression
    return parseExpr(sym, &first, owner);

  case TOK_STATIC:
    // static possibly followed by const
    while(sym.getToken() == TOK_STATIC) {
      qtStatic = true;
      nextSignificantToken(sym);
    }
    while(sym.getToken() == TOK_CONST) {
      qtConst = true;
      nextSignificantToken(sym);
    }
    // fall thru to const (refuse to use goto here)
  case TOK_CONST:
    // const possibly followed by static
    while(sym.getToken() == TOK_CONST) {
      qtStatic = true;
      nextSignificantToken(sym);
    }
    while(sym.getToken() == TOK_STATIC) {
      qtConst = true;
      nextSignificantToken(sym);
    }
    // fall thru to getting type
    
  case TOK_IDENTIFIER:
    // either var decl stmt or func decl
    {
      Type* t = module.findType(first);
      
      std::string second = *sym.getSymbolData()->getAsString();
      nextSignificantToken(sym);
      switch(sym.getToken()) {
      case TOK_EOF:
	fatalError("Unexpected EOF", sym);
	return std::unique_ptr<Stmt>(nullptr);
      case TOK_COMMA:
      case TOK_SEMICOLON:
      case TOK_ASSIGN:
	{
	  // var decl stmt
	  std::unique_ptr<VarDeclStmt> ret(new VarDeclStmt(owner));
	  std::shared_ptr<QualType> qt(new QualType(t));
	  qt->setConst(qtConst);
	  qt->setStatic(qtStatic);
	  ret->setType(std::move(qt));
	  parseVarDeclStmt(sym, ret.get(), &second);
	  return ret;
	}
      case TOK_OPEN_PAREN:
	{
	  // function decl
	  std::unique_ptr<FunctionDecl> ret(new FunctionDecl(owner->findScope()));
	  std::unique_ptr<QualType> qt(new QualType(t));
	  qt->setConst(qtConst);
	  qt->setStatic(qtStatic);
	  ret->setReturnType(std::move(qt));
	  ret->setName(std::make_unique<std::string>(second));
	  parseFunctionDeclTop(sym, ret.get());
	  return ret;
	}
      default:
	storeError("WTF in unknown stmt", sym);
	consumeObject(sym);
	return std::unique_ptr<Stmt>(nullptr);
      }
    }
  default:
    storeError("WTF in unknown stmt", sym);
    consumeObject(sym);
    return std::unique_ptr<Stmt>(nullptr);
  }
}

void Parser::parseVarDeclStmt(Symbol& sym, VarDeclStmt* vds,
			      std::string* id) {
  std::shared_ptr<QualType> qt(vds->getType());
  std::unique_ptr<VarDecl> vd(new VarDecl(vds->findScope(), std::move(qt)));
  if(id) {
    vd->setName(std::make_unique<std::string>(*id));
  }
  else {
    if(sym.getToken() != TOK_IDENTIFIER) {
      fatalError("parseVarDeclStmt not given id & sym is not an id", sym);
      return;
    }
    vd->setName(std::make_unique<std::string>(*sym.getSymbolData()->getAsString()));
    nextSignificantToken(sym);
  }
  vds->addVar(std::move(vd));

  while(true) { // I hate this and I want it gone
    switch(sym.getToken()) {
    case TOK_EOF:
      fatalError("Unexpected EOF", sym);
      return;
    case TOK_COMMA:
      nextSignificantToken(sym);
      if(sym.getToken() != TOK_IDENTIFIER) {
	storeError("Comma in var decl statement but no following decl", sym);
	consumeObject(sym);
	return;
      }
      vd = std::make_unique<VarDecl>(vds->findScope(), std::move(qt));
      vd->setName(std::make_unique<std::string>(*sym.getSymbolData()->getAsString()));
      vds->addVar(std::move(vd));
      nextSignificantToken(sym);
      continue;
    case TOK_SEMICOLON:
      return;
    default:
      storeError("WTF in var decl statement", sym);
      consumeObject(sym);
      return;
    }
  }

  return;
}

void Parser::consumeObject(Symbol& sym) {
  int brackets = 0;
  while(brackets || sym.getToken() != TOK_SEMICOLON) {
    if(sym.getToken() == TOK_OPEN_BRACKET) {
      brackets++;
    }
    if(sym.getToken() == TOK_CLOSE_BRACKET) {
      brackets--;
    }
    nextSignificantToken(sym);
  }
  return;
}

void Parser::nextSignificantToken(Symbol& sym) {
  do {
    sym = lex.next();
  } while(sym.getToken() == TOK_WHITESPACE ||
	  sym.getToken() == TOK_COMMENT);
  return;
}

void Parser::storeError(const std::string& errstr, const Symbol& sym) {
  ParserError err(errstr, filename, sym);
  errors.push_back(err);
  return;
}

void Parser::fatalError(const std::string& errstr, const Symbol& sym) {
  exit(-1);
}

void Parser::storeWarn(const std::string& errstr, const Symbol& sym) {
  ParserError warn(errstr, filename, sym);
  warnings.push_back(warn);
  return;
}

void Parser::setFile(const std::string& filename) {
  this->filename = filename;
  return;
}

void Parser::parse() {
  parseTop();
  return;
}
