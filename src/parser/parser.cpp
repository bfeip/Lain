#include "parser.hpp"

void Parser::parseTop() {
  Symbol sym;
  nextSignificantToken(sym);
  while(sym.getToken() != TOK_EOF) {
    switch(sym.getToken()) {
    case TOK_USING:
      nextSignificantToken(sym);
      if(sym.getToken() != TOK_STR_LIT) {
	storeError("Expected string", sym);
	consumeObject(sym);
	break;
      }
      module->addUsing(*sym.getSymbolData()->getAsString());
      nextSignificantToken(sym);
      if(sym.getToken() != TOK_SEMICOLON) {
	storeError("Expected semicolon", sym);
      }
      break;
    case TOK_CLASS:
      parseClassTop(sym, dynamic_cast<ScopeCreator*>(module.get()));
      break;
    case TOK_TYPEDEF:
      parseTypedef(sym, dynamic_cast<ScopeCreator*>(module.get()));
      break;
    case TOK_IDENTIFIER:
      parseIdDecl(sym, dynamic_cast<ScopeCreator*>(module.get()));
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
  std::unique_ptr<Type> type(new Type(std::string(), std::move(ast_classdecl)));
  owner->addOwnedType(std::move(type), AM_VOID);

  nextSignificantToken(sym);
  if(sym.getToken() == TOK_IDENTIFIER) {
    // named class
    const std::string& classname = *sym.getSymbolData()->getAsString();
    if(const Decl* d = owner->findDecl(classname)) {
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
    Type* inher = module->findType(inhername);
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
    std::vector<std::unique_ptr<VarDecl>> vars = vds->stripVars();
    for(std::unique_ptr<VarDecl>& e : vars) {
      module->addGlobal(std::move(e));
    }
    return;
  }

  if(ClassDecl* owningClass = dynamic_cast<ClassDecl*>(owner)) {
    // member variable instantation
    parseVarDeclStmt(sym, vds.get(), nullptr);
    std::vector<std::unique_ptr<VarDecl>> vars = vds->stripVars();
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
  std::unique_ptr<Type> type(new Type(std::string(), std::move(ast_tdd)));
  owner->addOwnedType(std::move(type), AM_VOID);

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
  tdd->setDefinition(owner->findType(*sym.getSymbolData()->getAsString()));

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
  Type* t = module->findType(typeName);
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
  const std::string name = *sym.getSymbolData()->getAsString();

  // determine what kind of decl (function or var)
  nextSignificantToken(sym);

  switch(sym.getToken()) {
  case TOK_OPEN_PAREN:
    // Function or method decl
    {
      FunctionDecl* fd = owner->findFunctionDecl(name);
      if(fd == nullptr) {
	// this is a new function decl
	std::unique_ptr<FunctionDecl> ast_fd(new FunctionDecl(owner));
	fd = ast_fd.get();
	owner->addOwnedFunction(std::move(ast_fd), AM_VOID); // TODO AM stuff
	fd->setName(std::make_unique<std::string>(name));
	fd->setReturnType(std::move(qt));
      }
      parseFunctionDeclTop(sym, fd);
      return;
    }
  case TOK_COMMA:
  case TOK_ASSIGN:
    // global var decl stmt or member decl stmt
    {
      std::unique_ptr<VarDeclStmt> vds(new VarDeclStmt(owner));
      std::string typeName = name;
      vds->setType(std::shared_ptr<QualType>(qt.release()));
      parseVarDeclStmt(sym, vds.get(), &typeName);
      if(dynamic_cast<Module*>(owner)) {
        // gloabal var decl stmt
        std::vector<std::unique_ptr<VarDecl>> vars = vds->stripVars();
        for(std::unique_ptr<VarDecl>& e : vars) {
          module->addGlobal(std::move(e));
        }
      }
      else {
        // member var decl stmt
        ClassDecl* cd = dynamic_cast<ClassDecl*>(owner);
        std::vector<std::unique_ptr<VarDecl>> vars = vds->stripVars();
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
  if(sym.getToken() != TOK_CLOSE_PAREN) {
    storeError("Something when wrong with this function decl", sym);
    consumeObject(sym);
    return;
  }
  nextSignificantToken(sym);

  switch(sym.getToken()) {
  case TOK_EOF:
    fatalError("Unexpected EOF", sym);
    return;
  case TOK_SEMICOLON:
    // function prototype
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
    Type* t = module->findType(typeName);
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
    case TOK_CLOSE_PAREN:
      break;
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
	parseCompoundStmt(sym, stmt.get());
        cs->addStmt(std::move(stmt));
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
        std::unique_ptr<Expr> stmt = parseExpr(sym, nullptr, cs, nullptr);
        cs->addStmt(std::move(stmt));
      }
      break;

    case TOK_IF:
      // if stmt
      {
      	std::unique_ptr<IfStmt> stmt(new IfStmt(cs));
	parseIfStmt(sym, stmt.get());
        cs->addStmt(std::move(stmt));
      }
      break;

    case TOK_ELSE:
      {
	IfStmt* ifstmt = dynamic_cast<IfStmt*>(cs->getStmts().back());
	while(ElseStmt* elsestmt = ifstmt->getElse()) {
	  // otherwise this'd get the FIRST if stmt that appered, we want the LAST
	  ifstmt = dynamic_cast<IfStmt*>(elsestmt->getBody());
	  if(!ifstmt) {
	    storeError("Else ststement without if", sym);
	    break;
	  }
	}
	    
        if(!ifstmt) {
          storeError("Else statement without if", sym);
          break;
        }
        std::unique_ptr<ElseStmt> stmt(new ElseStmt(cs));
        stmt->setIf(ifstmt);
        parseElseStmt(sym, stmt.get());
	ifstmt->setElse(std::move(stmt));
      }
      break;

    case TOK_WHILE:
      {
        std::unique_ptr<WhileStmt> stmt(new WhileStmt(cs));
        parseWhileStmt(sym, stmt.get());
	cs->addStmt(std::move(stmt));
      }
      break;

    case TOK_FOR:
      {
        std::unique_ptr<ForStmt> stmt(new ForStmt(cs));
        parseForStmt(sym, stmt.get());
	cs->addStmt(std::move(stmt));
      }
      break;

    case TOK_SWITCH:
      {
        std::unique_ptr<SwitchStmt> stmt(new SwitchStmt(cs));
        parseSwitchStmt(sym, stmt.get());
	cs->addStmt(std::move(stmt));
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
        nextSignificantToken(sym);
        if(sym.getToken() == TOK_IDENTIFIER ||
           sym.getToken() == TOK_NUM_LIT ||
           sym.getToken() == TOK_STR_LIT ||
           sym.getToken() == TOK_CHAR_LIT) {
          stmt->setValue(parseExpr(sym, nullptr, stmt, nullptr));
        }
	cs->addStmt(std::move(ast_stmt));
      }
      break;

    case TOK_ENUM:
      // parseEnum(sym, cs); TODO: write parseEnum()
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
                                        ScopeCreator* owner, Expr* parentExpr) {
  std::unique_ptr<Expr> ret;
  std::string identifier = "";
  std::string literal = "";
  LiteralKind literalKind;

  // check if this a grouped expression

  // get id if we were passed one, else check if sym is an id
  if(id) {
    identifier = *id;
  }
  if(sym.getToken() == TOK_IDENTIFIER) {
    if(id) {
      storeError("Two identifiers in a row?", sym);
      consumeObject(sym);
      return std::unique_ptr<Expr>(nullptr);
    }
    identifier = *sym.getSymbolData()->getAsString();
    nextSignificantToken(sym);
  }

  // check if sym is a literal of some kind
  if(sym.getToken() == TOK_STR_LIT ||
     sym.getToken() == TOK_NUM_LIT ||
     sym.getToken() == TOK_CHAR_LIT) {
    if(id) {
      storeError("Identifier followed by literal?", sym);
      consumeObject(sym);
      return std::unique_ptr<Expr>(nullptr);
    }
    literal = *sym.getSymbolData()->getAsString();
    switch(sym.getToken()) {
    case TOK_STR_LIT:
      literalKind = LK_STR;
      break;
    case TOK_CHAR_LIT:
      literalKind = LK_CHAR;
      break;
    case TOK_NUM_LIT:
      literalKind = LK_NUM;
      break;
    }
    nextSignificantToken(sym);
  }

  // get pre-identifier ops
  OperationType preOp = OP_VOID;
  if(identifier == "" && literal == "") {
    // for now only one of these ops is allowed at a time
    switch(sym.getToken()) {
    case TOK_EOF:
      fatalError("Unexpected EOF", sym);
      return std::unique_ptr<Expr>(nullptr);
    case TOK_ADD:
      preOp = OP_ABS;
      break;
    case TOK_SUB:
      preOp = OP_NEG;
      break;
    case TOK_INC:
      preOp = OP_PRE_INC;
      break;
    case TOK_DEC:
      preOp = OP_PRE_DEC;
      break;
    case TOK_NOT:
      preOp = OP_NOT;
      break;
    default:
      storeError("Unexpected token in expr", sym);
      consumeObject(sym);
      return std::unique_ptr<Expr>(nullptr);
    }
    nextSignificantToken(sym);

    // we can't leave this block without either a lvalue, rvalue, or open paren
    switch(sym.getToken()) {
    case TOK_EOF:
      fatalError("Unexpected EOF", sym);
      return std::unique_ptr<Expr>(nullptr);
    case TOK_IDENTIFIER:
      identifier = *sym.getSymbolData()->getAsString();
      nextSignificantToken(sym);
      break;
    case TOK_STR_LIT:
      literal = *sym.getSymbolData()->getAsString();
      literalKind = LK_STR;
      nextSignificantToken(sym);
      break;
    case TOK_CHAR_LIT:
      literal = *sym.getSymbolData()->getAsString();
      literalKind = LK_CHAR;
      nextSignificantToken(sym);
      break;
    case TOK_NUM_LIT:
      literal = *sym.getSymbolData()->getAsString();
      literalKind = LK_NUM;
      nextSignificantToken(sym);
      break;
    case TOK_OPEN_PAREN:
      break;
    default:
      storeError("Unexpected token after pre-op", sym);
      consumeObject(sym);
      return std::unique_ptr<Expr>(nullptr);
    }
  }

  // by now we have either a literal or an identifier
  // we're ignoring the dot operator and array access here for now
  if(identifier != "") {
    // this is a function call or a variable
    if(sym.getToken() == TOK_OPEN_PAREN) {
      FunctionDecl* funcDecl = owner->findFunctionDecl(identifier);
      std::unique_ptr<FunctionCallExpr> func(new FunctionCallExpr(owner, parentExpr, funcDecl));
      nextSignificantToken(sym);
      while(true) {
        // grab args seperated by commas and ending in a close paren
        std::unique_ptr<Expr> arg = parseExpr(sym, nullptr, owner, func.get());
	if(arg) {
	  func->addArg(std::move(arg));
	}
	else {
	  break;
	}
        if(sym.getToken() == TOK_COMMA) {
          nextSignificantToken(sym);
          continue;
        }
        else if(sym.getToken() == TOK_CLOSE_PAREN) {
          nextSignificantToken(sym);
          break;
        }
        else {
          storeError("Unexpected token in argument", sym);
          consumeObject(sym);
          return std::unique_ptr<Expr>(nullptr);
        }
      }
      ret = std::move(func);
    }
    else {
      ret = std::make_unique<VarInstanceExpr>(owner, parentExpr, owner->findVarDecl(identifier));
    }
  }
  else if(literal != "") {
    // this is a literal
    ret = std::make_unique<LiteralExpr>(owner, parentExpr, literal, literalKind);
  }
  else if(sym.getToken() == TOK_OPEN_PAREN) {
    // this is a grouped expression
    nextSignificantToken(sym);
    std::unique_ptr<GroupedExpr> group(new GroupedExpr(owner, parentExpr, nullptr));
    std::unique_ptr<Expr> body = parseExpr(sym, nullptr, owner, group.get());
    if(sym.getToken() != TOK_CLOSE_PAREN) {
      storeError("Grouped expression must end in a close paren", sym);
      consumeObject(sym);
      return std::unique_ptr<Expr>(nullptr);
    }
    group->setBody(std::move(body));
    ret = std::move(group);
    nextSignificantToken(sym);
  }

  // apply the preOp if there was one
  if(preOp != OP_VOID) {
    ret = std::make_unique<UnaryOperationExpr>(owner, parentExpr, std::move(ret), preOp);
  }

  // get postop stuff is there is any
  OperationType postOp = OP_VOID;
  switch(sym.getToken()) {
  case TOK_EOF:
    fatalError("Unexpected EOF", sym);
    return std::unique_ptr<Expr>(nullptr);
  case TOK_INC:
    postOp = OP_POST_INC;
    nextSignificantToken(sym);
    break;
  case TOK_DEC:
    postOp = OP_POST_DEC;
    nextSignificantToken(sym);
    break;
  }

  // apply the postOp if there was one
  if(postOp != OP_VOID) {
    ret = std::make_unique<UnaryOperationExpr>(owner, parentExpr, std::move(ret), postOp);
  }

  // if this is part of a bianry expr, get the other part recursivly
  // It should be noted that when an expr is added as an operand, that expr's
  // parentExpr is changed to be the binary expr
  switch(sym.getToken()) {
  case TOK_EOF:
    fatalError("Unexpected EOF", sym);
    return std::unique_ptr<Expr>(nullptr);
  case TOK_ASSIGN:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_ASSIGN));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_ADD:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_ADD));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;

    }
  case TOK_SUB:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_SUB));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_MUL:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_MUL));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_DIV:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_DIV));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_MOD:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_MOD));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_ADD_ASSIGN:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_ADD_ASSIGN));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_SUB_ASSIGN:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_SUB_ASSIGN));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_MUL_ASSIGN:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_MUL_ASSIGN));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_DIV_ASSIGN:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_DIV_ASSIGN));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_MOD_ASSIGN:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_MOD_ASSIGN));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_AND:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_AND));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_OR:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_OR));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_XOR:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_XOR));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_LSH:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_LSH));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_RSH:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_RSH));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_AND_ASSIGN:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_AND_ASSIGN));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_OR_ASSIGN:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_OR_ASSIGN));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_XOR_ASSIGN:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_XOR_ASSIGN));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_LSH_ASSIGN:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_LSH_ASSIGN));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_RSH_ASSIGN:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_RSH_ASSIGN));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_EQ:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_EQ));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_NEQ:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_NEQ));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_LT:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_LT));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_GT:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
								       nullptr, OP_GT));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_LOGI_AND:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_LOGI_AND));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_LOGI_OR:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_LOGI_OR));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  case TOK_LOGI_XOR:
    {
      nextSignificantToken(sym);
      std::unique_ptr<BinaryOperationExpr> boe(new BinaryOperationExpr(owner, parentExpr,
								       std::move(ret),
                                                                       nullptr, OP_LOGI_XOR));
      std::unique_ptr<Expr> rOperand = parseExpr(sym, nullptr, owner, boe.get());
      boe->setRightOperand(std::move(rOperand));
      ret = std::move(boe);
      break;
    }
  }

  // the previous switch should have recursivly eaten add the binary operators
  // now we see if their is and indication of an end
  switch(sym.getToken()) {
  case TOK_EOF:
    fatalError("Unexpected EOF", sym);
    return std::unique_ptr<Expr>(nullptr);
  case TOK_CLOSE_PAREN:
  case TOK_CLOSE_SQUARE_BRACKET:
  case TOK_COMMA:
  case TOK_SEMICOLON:
    return ret;
  default:
    storeError("Something unexpected happened", sym);
    consumeObject(sym);
    return std::unique_ptr<Expr>(nullptr);
  }
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
  std::unique_ptr<Expr> if_expr(parseExpr(sym, nullptr, stmt, nullptr));
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
  std::unique_ptr<Expr> while_expr(parseExpr(sym, nullptr, stmt, nullptr));
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
    {
      std::unique_ptr<Stmt> start(dynamic_cast<Stmt*>(parseIdStmt(sym, stmt).release()));
      stmt->setStart(std::move(start));
    }
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
    std::unique_ptr<Expr> for_stopExpr(parseExpr(sym, nullptr, stmt, nullptr));
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
    std::unique_ptr<Expr> for_stepExpr(parseExpr(sym, nullptr, stmt, nullptr));
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
  std::unique_ptr<Expr> switch_expr(parseExpr(sym, nullptr, stmt, nullptr));
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

std::unique_ptr<AstNode> Parser::parseIdStmt(Symbol& sym, ScopeCreator* owner) {
  bool qtStatic = false;
  bool qtConst = false;
  if(sym.getToken() != TOK_IDENTIFIER) {
    fatalError("parseIdStmt not passed identifier", sym);
    return std::unique_ptr<AstNode>(nullptr);
  }
  std::string first = *sym.getSymbolData()->getAsString();

  nextSignificantToken(sym);
  switch(sym.getToken()) {
  case TOK_EOF:
    fatalError("Unexpected EOF", sym);
    return std::unique_ptr<AstNode>(nullptr);

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
    return parseExpr(sym, &first, owner, nullptr);

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
      Type* t = module->findType(first);

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
          ret->setType(qt);
          parseVarDeclStmt(sym, ret.get(), &second);
          return std::move(ret);
        }
      case TOK_OPEN_PAREN:
        {
          // function decl
	  FunctionDecl* fd = owner->findFunctionDecl(second);
	  if(fd == nullptr) {
	    std::unique_ptr<FunctionDecl> ret(new FunctionDecl(owner));
	    std::unique_ptr<QualType> qt(new QualType(t));
	    qt->setConst(qtConst);
	    qt->setStatic(qtStatic);
	    ret->setReturnType(std::move(qt));
	    ret->setName(std::make_unique<std::string>(second));
	    parseFunctionDeclTop(sym, ret.get());
	    return ret;
	  }
	  else {
	    parseFunctionDeclTop(sym, fd);
	    return std::unique_ptr<AstNode>(nullptr);
	  }
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
  std::unique_ptr<VarDecl> vd(new VarDecl(vds->getOwner(), qt));
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
      
  if(sym.getToken() == TOK_ASSIGN) {
    nextSignificantToken(sym);
    std::unique_ptr<Expr> init = parseExpr(sym, nullptr, vds, nullptr);
    vd->setInit(std::move(init));
  }
  vds->getOwner()->addOwnedVar(vd.get());
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
      vd = std::make_unique<VarDecl>(vds->getOwner(), qt);
      vd->setName(std::make_unique<std::string>(*sym.getSymbolData()->getAsString()));
      vds->getOwner()->addOwnedVar(vd.get());
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

void Parser::parse() {
  parseTop();
  return;
}

std::unique_ptr<Module>&& Parser::stripModule() {
  return std::move(module);
}
