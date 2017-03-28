#include "emitter.hpp"

void Emitter::emitTop() {
  // emit TypeDecls first
  /* I'm not sure how LLVM goes about type decls and dependencies, so we
   * may eventually end up doing all the declarations first and the 
   * definitions later. If I end up choosing to do that this is where
   * I'll insert a emitDecls call */
  const std::vector<const Type*>& types = tu->getTypes();
  for(const Type* type : types) {
    if(!type->isPrimitive()) {
      emitType(type->getDecl());
    }
  }

  // emit globals next
  const std::vector<const VarDecl*>& globals = tu->getGlobals();
  for(const VarDecl* global : globals) {
    emitGlobalVar(global);
  }

  // emit FuntionDecls last
  const std::vector<const FunctionDecl*>& funcs = tu->getFunctionDecls();
  for(const FunctionDecl* func : funcs) {
    // emit declatarions
    emitFunctionDec(func);
  }
  for(const FunctionDecl* func : funcs) {
    // emit definitions
    emitFunctionDef(func);
  }

  return;
}

void Emitter::emitGlobalVar(const VarDecl* vd) {
  llvm::Type* type = findType(vd->getType()->getType()->getName());
  const std::string& name = *vd->getName();
  llvm::Constant* init = static_cast<llvm::Constant*>(emitExpr(vd->getInit()));
  std::unique_ptr<llvm::GlobalVariable> global(new llvm::GlobalVariable(*module.get(), type, false,
									llvm::GlobalValue::ExternalLinkage,
									init, name));
  globalVars.emplace_back(std::move(global));
  return;
} 

void Emitter::emitType(const TypeDecl* td) {
  if(const ClassDecl* cd = dynamic_cast<const ClassDecl*>(td)) {
    emitClass(cd);
    return;
  }
  //if(const EnumDecl* ed = dynamic_cast<const EnumDecl*>(ed)) {
  //  emitEnum(ed);                                                 TODO
  //  return;
  //}
  // ignoring typedefs
  return;
}

void Emitter::emitClass(const ClassDecl* cd) {
  llvm::StructType* s;
  if(!cd->getName() || *cd->getName() == "") {
    // anonomous class
    /* I'm thinking I'll just use these literally in the LLVM IR. For
     * example, instead of declaring an _AnonomousStruct0 struct and
     * passing that around I'll just pass it around as { i32, i32 }*
     * (or whatever the members happen to be). The following is how
     * one could make an anonomous struct, just in case */
    // s = llvm::StructType::get(context);
  }
  else {
    // named class
    s = llvm::StructType::create(context, *cd->getName());
    classMap.insert(std::make_pair(*cd->getName(), s));
  }

  // emit contained types
  const std::vector<const Type*>& types = cd->getTypes();
  for(const Type* type : types) {
    if(!type->isPrimitive()) {
      emitType(type->getDecl());
    }
  }

  // emit methods
  const std::vector<const FunctionDecl*>& funcs = cd->getFunctionDecls();
  for(const FunctionDecl* fd : funcs) {
    emitMethod(fd, s);
  }
  
  return;
}

void Emitter::emitFunctionDec(const FunctionDecl* fd) {
  // get param types
  const std::vector<const VarDecl*> lainParams = fd->getParams();
  std::vector<llvm::Type*> llvmParams;
  for(const VarDecl* vd : lainParams) {
    // find Type of vd (not QualType)
    llvm::Type* llvmType = findType(vd->getType()->getType()->getName());
    if(!llvmType) {
      fatalError("could not find corrosponding LLVM type to " + *vd->getName());
    }
    llvmParams.push_back(llvmType);
  }

  // get return type
  const std::string& lainReturnType = fd->getReturnType()->getType()->getName();
  llvm::Type* llvmReturnType = findType(lainReturnType);

  // create FunctionType
  /* The last 'false' argument here relates to var args. If I was going to add
   * that feature this would need to be changed */
  llvm::FunctionType* ft = llvm::FunctionType::get(llvmReturnType, llvmParams, false);

  // create Function
  /* Just external linkage is used for now since I'm lazy and I don't know
   * how much of this is going to get changed. */
  const std::string* functionName = fd->getName();
  if(!functionName) {
    fatalError("All functions must have names");
  }
  llvm::Function* f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage,
					     *functionName, module.get());

  return;
}

void Emitter::emitFunctionDef(const FunctionDecl* fd) {
  llvm::Function* f = module->getFunction(*fd->getName());
  functionStack.push(f);

  // Add body to function
  llvm::BasicBlock* bb = llvm::BasicBlock::Create(context, "entry", f);
  builder.SetInsertPoint(bb);
  int i = 0;
  const std::vector<const VarDecl*> lainParams = fd->getParams();
  for(auto& arg : f->args()) {
    arg.setName(*lainParams[i++]->getName());
  }

  scopedValues.emplace_back();
  llvm::StringMap<llvm::Value*>& scope = scopedValues.back();
  for(auto& arg : f->args()) {
    scope[arg.getName()] = &arg;
  }

  emitCompoundStmt(fd->getBody()); // emit function body
  scopedValues.pop_back(); // remove params
  functionStack.pop();
  llvm::verifyFunction(*f);

  return;

}

void Emitter::emitMethod(const FunctionDecl* fd, llvm::StructType* self) {
  // get param types
  const std::vector<const VarDecl*>& lainParams = fd->getParams();
  std::vector<llvm::Type*> llvmParams;
  llvmParams.push_back(self); // add 'this' param
  for(const VarDecl* vd : lainParams) {
    // find Type of vd (not QualType)
    llvm::Type* llvmType = findType(vd->getType()->getType()->getName());
    if(!llvmType) {
      fatalError("could not find corrosponding LLVM type to " + *vd->getName());
    }
    llvmParams.push_back(llvmType);
  }

  // get return type
  const std::string& lainReturnType = fd->getReturnType()->getType()->getName();
  llvm::Type* llvmReturnType = findType(lainReturnType);

  // create FunctionType
  /* The last 'false' argument here relates to var args. If I was going to add
   * that feature this would need to be changed */
  llvm::FunctionType* ft = llvm::FunctionType::get(llvmReturnType, llvmParams, false);

  // create Function
  std::string functionName = "_Meth_" + self->getName().str() + "_" +
    *fd->getName();
  llvm::Function* f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage,
					     functionName, module.get());
  functionStack.push(f);

  // add body to function and set names of params
  llvm::BasicBlock* bb = llvm::BasicBlock::Create(context, "entry", f);
  builder.SetInsertPoint(bb);
  int i = 0;
  f->args().begin()->setName("this");
  auto argStart = f->args().begin();
  for(auto arg = argStart++; arg != f->args().end(); arg++) {
    arg->setName(*lainParams[i]->getName());
  }

  // add params to scoped values
  scopedValues.emplace_back();
  llvm::StringMap<llvm::Value*>& scope = scopedValues.back();
  for(auto& arg : f->args()) {
    scope[arg.getName()] = &arg;
  }

  emitCompoundStmt(fd->getBody()); // emit function body
  scopedValues.pop_back(); // remove params from scoped values
  functionStack.pop();
  llvm::verifyFunction(*f);

  return;
}

void Emitter::emitStmt(const Stmt* stmt) {
  if(const Expr* expr = dynamic_cast<const Expr*>(stmt)) {
    emitExpr(expr);
  }
  else if(const VarDeclStmt* varDeclStmt = dynamic_cast<const VarDeclStmt*>(stmt)) {
    emitVarDeclStmt(varDeclStmt);
  }
  else if(const IfStmt* ifStmt = dynamic_cast<const IfStmt*>(stmt)) {
    emitIfStmt(ifStmt);
  }
  else if(const WhileStmt* whileStmt = dynamic_cast<const WhileStmt*>(stmt)) {
    emitWhileStmt(whileStmt);
  }
  else if(const ForStmt* forStmt = dynamic_cast<const ForStmt*>(stmt)) {
    emitForStmt(forStmt);
  }
  else if(const SwitchStmt* switchStmt = dynamic_cast<const SwitchStmt*>(stmt)) {
    //    emitSwitchStmt(switchStmt); TODO
  }
  else if(const BreakStmt* breakStmt = dynamic_cast<const BreakStmt*>(stmt)) {
    emitBreakStmt(breakStmt);
  }
  else if(const ContinueStmt* continueStmt = dynamic_cast<const ContinueStmt*>(stmt)) {
    emitContinueStmt(continueStmt);
  }
  else if(const ReturnStmt* returnStmt = dynamic_cast<const ReturnStmt*>(stmt)) {
    emitReturnStmt(returnStmt);
  }
  else if(const CompoundStmt* compoundStmt = dynamic_cast<const CompoundStmt*>(stmt)) {
    emitCompoundStmt(compoundStmt);
  }
  else if(dynamic_cast<const NullStmt*>(stmt)) {
    return;
  }
  return;
}

void Emitter::emitCompoundStmt(const CompoundStmt* cs) {
  const std::vector<const Stmt*>& stmts = cs->getStmts();
  for(const Stmt* stmt : stmts) {
    emitStmt(stmt);
  }
  return;
}

void Emitter::emitVarDeclStmt(const VarDeclStmt* vds) {
  llvm::Type* type = findType(vds->getType()->getType()->getName());
  const std::vector<const VarDecl*>& vars = vds->getVars();
  llvm::StringMap<llvm::Value*>& scope = scopedValues.back();
  for(const VarDecl* var : vars) {
    // all vars are stack allocated right now
    llvm::Value* addr = builder.CreateAlloca(type, nullptr, *var->getName());
    scope.insert(std::pair<std::string, llvm::Value*>(*var->getName(), addr));

    // store the value of the init at the vars address
    if(const Expr* init = var->getInit()) {
      llvm::Value* initVal = emitExpr(init);
      builder.CreateStore(castType(initVal, addr->getType()->getPointerElementType()), addr);
    }
  }

  return;
}

void Emitter::emitIfStmt(const IfStmt* is) {
  llvm::Value* condition = emitExpr(is->getCondition());
  llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(context, "if_exit",
							functionStack.top());
  llvm::BasicBlock* ifBlock = llvm::BasicBlock::Create(context, "if",
						       functionStack.top());
  llvm::BasicBlock* elseBlock = nullptr;
  const ElseStmt* es = is->getElse();

  // create apropriate conditional br
  if(es) {
    elseBlock = llvm::BasicBlock::Create(context, "else", functionStack.top());
    builder.CreateCondBr(removeRefs(condition), ifBlock, elseBlock);
  }
  else {
    builder.CreateCondBr(removeRefs(condition), ifBlock, endBlock);
  }

  // emit if body
  builder.SetInsertPoint(ifBlock);
  emitStmt(is->getBody());
  if(ifBlock->back().isTerminator()) {
    // if the last instruction properly ends the block
    builder.CreateUnreachable();
  }
  else {
    // block not properly ended
    builder.CreateBr(endBlock);
    builder.CreateUnreachable();
  }

  // emit else body if there is one
  if(es) {
    builder.SetInsertPoint(elseBlock);
    emitStmt(es->getBody());
    if(elseBlock->back().isTerminator()) {
      // if the last instruction properly ends the block
      builder.CreateUnreachable();
    }
    else {
      // block not properly ended
      builder.CreateBr(endBlock);
      builder.CreateUnreachable();
    }
  }

  builder.SetInsertPoint(endBlock);
  return;
}

void Emitter::emitWhileStmt(const WhileStmt* ws) {
  llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(context, "while_exit",
							functionStack.top());
  llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(context, "while_cond",
							 functionStack.top());
  llvm::BasicBlock* whileBlock = llvm::BasicBlock::Create(context, "while",
							  functionStack.top());

  // create condition
  builder.CreateBr(condBlock);
  builder.SetInsertPoint(condBlock);
  llvm::Value* condition = emitExpr(ws->getCondition());
  builder.CreateCondBr(condition, whileBlock, endBlock);
  builder.CreateUnreachable();

  // create while body
  builder.SetInsertPoint(whileBlock);
  breakTo.push(endBlock);
  continueTo.push(condBlock);
  emitStmt(ws->getBody());
  builder.CreateBr(condBlock);
  builder.CreateUnreachable();

  builder.SetInsertPoint(endBlock);
  breakTo.pop();
  continueTo.pop();
  return;
}

void Emitter::emitForStmt(const ForStmt* fs) {
  llvm::BasicBlock* initBlock = llvm::BasicBlock::Create(context, "for_init",
							 functionStack.top());
  llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(context, "for_cond",
							 functionStack.top());
  llvm::BasicBlock* stepBlock = llvm::BasicBlock::Create(context, "for_step",
							 functionStack.top());
  llvm::BasicBlock* forBlock = llvm::BasicBlock::Create(context, "for",
							functionStack.top());
  llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(context, "for_exit",
							functionStack.top());
  // create init
  builder.CreateBr(initBlock);
  builder.SetInsertPoint(initBlock);
  emitStmt(fs->getStart());
  builder.CreateBr(condBlock);

  // create condition
  builder.SetInsertPoint(condBlock);
  llvm::Value* condition = emitExpr(fs->getStop());
  builder.CreateCondBr(condition, forBlock, endBlock);

  // create step
  builder.SetInsertPoint(stepBlock);
  emitExpr(fs->getStep());
  builder.CreateBr(condBlock);

  // create for body
  builder.SetInsertPoint(forBlock);
  breakTo.push(endBlock);
  continueTo.push(stepBlock);
  emitStmt(fs->getBody());
  builder.CreateBr(stepBlock);
  builder.CreateUnreachable();

  breakTo.pop();
  continueTo.pop();
  builder.SetInsertPoint(endBlock);
  return;
}

/*void Emitter::emitSwitchStmt(const SwitchStmt* ss) {
  llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(context, "",
							functionStack.top());
  llvm::BasicBlock* defaultBlock = nullptr;
  if(const std::pair<int, const CompoundStmt*>* def = ss->getDefault()) {
    defaultBlock = llvm::BasicBlock::Create(context, "", functionStack.top());
  }
  const std::vector<const std::pair<int, const CompoundStmt*>>& cases = ss->getCases();

  // create switch
  llvm::Value* condition = emitExpr(ss->getCondition());
  llvm::SwitchInst* swi = nullptr;
  if(defaultBlock) {
    swi = builder.CreateSwitch(condition, defaultBlock);
  }
  else {
    swi = builder.CreateSwitch(condition, endBlock);
  }

  // add cases to switch
  breakTo.push(endBlock);
  for(const std::pair<int, const CompoundStmt*>& c : cases) {
    llvm::BasicBlock* caseBlock = llvm::BasicBlock::Create(LLVMContext, "",
							   functionStack.peek());
    builder.SetInsertPoint(caseBlock);
    emitCompoundStmt(c.second());
    builder.CreateBr(endBlock);
    swi->addCase(c->first(), caseBlock);
  }

  breakTo.pop();
  builder.SetInsertPoint(endBlock);
  return;
}*/

void Emitter::emitBreakStmt(const BreakStmt* bs) {
  builder.CreateBr(breakTo.top());
  return;
}

void Emitter::emitContinueStmt(const ContinueStmt* cs) {
  builder.CreateBr(continueTo.top());
  return;
}

void Emitter::emitReturnStmt(const ReturnStmt* rs) {
  if(const Expr* retExpr = rs->getValue()) {
    llvm::Value* ret = emitExpr(rs->getValue());
    builder.CreateRet(ret);
  }
  else {
    builder.CreateRetVoid();
  }
  return;
}

llvm::Value* Emitter::emitExpr(const Expr* e) {
  llvm::Value* ret;
  if(const BinaryOperationExpr* boe = dynamic_cast<const BinaryOperationExpr*>(e)) {
    ret = emitBinaryOperationExpr(boe);
  }
  else if(const UnaryOperationExpr* uoe = dynamic_cast<const UnaryOperationExpr*>(e)) {
    ret = emitUnaryOperationExpr(uoe);
  }
  else if(const FunctionCallExpr* fce = dynamic_cast<const FunctionCallExpr*>(e)) {
    ret = emitFunctionCallExpr(fce);
  }
  else if(const GroupedExpr* ge = dynamic_cast<const GroupedExpr*>(e)) {
    ret = emitExpr(ge->getBody());
  }
  else if(const VarInstanceExpr* vie = dynamic_cast<const VarInstanceExpr*>(e)) {
    ret = emitVarInstanceExpr(vie);
  }
  else if(const LiteralExpr* le = dynamic_cast<const LiteralExpr*>(e)) {
    ret = emitLiteralExpr(le);
  }
  else {
    fatalError("Missing expr case in emitter");
  }
  for(llvm::Instruction* inst : delayedInstructions) {
    builder.Insert(inst);
  }
  delayedInstructions.clear();
  return ret;
}

llvm::Value* Emitter::emitBinaryOperationExpr(const BinaryOperationExpr* boe) {
  // assumes operations are stored in the correct order
  // in all of this there might need to be casts and stuff, we won't worry about that
  // for now
  llvm::Value* l = emitExpr(boe->getLeftOperand());
  llvm::Value* r = emitExpr(boe->getRightOperand());
  std::pair<llvm::Value*, llvm::Value*> corrected;
  switch(boe->getOp()) {
  case OP_ASSIGN:
    {
      // array access ignored
      const Expr* leftOp = boe->getLeftOperand();
      r = castType(r, l->getType()->getPointerElementType());
      return builder.CreateStore(r, l);
    }
  case OP_ADD:
    corrected = readyBOE(boe, l, r);
    return builder.CreateAdd(corrected.first, corrected.second);
  case OP_SUB:
    corrected = readyBOE(boe, l, r);
    return builder.CreateSub(corrected.first, corrected.second);
  case OP_MUL:
    corrected = readyBOE(boe, l, r);
    return builder.CreateMul(corrected.first, corrected.second);
  case OP_DIV:
    corrected = readyBOE(boe, l, r);
    return builder.CreateSDiv(corrected.first, corrected.second);
  case OP_MOD:
    corrected = readyBOE(boe, l, r);
    return builder.CreateSRem(corrected.first, corrected.second);
  case OP_LT:
    corrected = readyBOE(boe, l, r);
    return builder.CreateICmpSLT(corrected.first, corrected.second);
  case OP_GT:
    corrected = readyBOE(boe, l, r);
    return builder.CreateICmpSGT(corrected.first, corrected.second);
  case OP_LOGI_AND:
    corrected = readyBOE(boe, l, r);
    l = builder.CreateICmpNE(corrected.first,
			     llvm::ConstantInt::get(corrected.first->getType(), 0));
    r = builder.CreateICmpNE(corrected.second,
			     llvm::ConstantInt::get(corrected.second->getType(), 0));
    return builder.CreateAnd(l, r);
  case OP_LOGI_OR:
    corrected = readyBOE(boe, l, r);
    l = builder.CreateICmpNE(corrected.first,
			     llvm::ConstantInt::get(corrected.first->getType(), 0));
    r = builder.CreateICmpNE(corrected.second,
			     llvm::ConstantInt::get(corrected.second->getType(), 0));
    return builder.CreateAnd(l, r);
  default:
    fatalError("Missing binary expr case in emitter");
    // TODO: add the rest of the operations
  }
}

llvm::Value* Emitter::emitUnaryOperationExpr(const UnaryOperationExpr* uoe) {
  const Expr* operand = uoe->getOperand();
  llvm::Value* child;
  llvm::Value* updated;
  llvm::BinaryOperator* delayed;
  static llvm::ConstantInt* one = llvm::ConstantInt::get(llvm::IntegerType::get(context, 2), 1);
  switch(uoe->getOp()) {
  case OP_PRE_INC:
    child = emitExpr(operand);
    child = builder.CreateLoad(child);
    updated = builder.CreateAdd(child, castType(one, child->getType()));
    return builder.CreateStore(updated, child);
  case OP_PRE_DEC:
    child = emitExpr(operand);
    child = builder.CreateLoad(child);
    updated = builder.CreateSub(child, castType(one, child->getType()));
    return builder.CreateStore(updated, child);
  case OP_POST_INC:
    child = emitExpr(operand);
    child = builder.CreateLoad(child);
    delayed = llvm::BinaryOperator::Create(llvm::Instruction::Add, child,
					   castType(one, child->getType()));
    delayedInstructions.push_back(delayed);
    return child;
  case OP_POST_DEC:
    child = emitExpr(operand);
    child = builder.CreateLoad(child);
    delayed = llvm::BinaryOperator::Create(llvm::Instruction::Sub, child,
					   castType(one, child->getType()));
    delayedInstructions.push_back(delayed);
    return child;
  case OP_NEG:
    child = emitExpr(operand);
    child = builder.CreateLoad(child);
    return builder.CreateNeg(child);
  case OP_ABS:
    // TODO
  case OP_NOT:
    child = emitExpr(operand);
    child = removeRefs(child);
    return builder.CreateICmpNE(child, llvm::ConstantInt::get(child->getType(), 0));
  default:
    fatalError("Missing unary expr case in emitter");
  }
}

llvm::Value* Emitter::emitFunctionCallExpr(const FunctionCallExpr* fce) {
  // Too easy, not accounting for function overrideing
  return builder.CreateCall(module->getFunction(*fce->getFunc()->getName()));
}

llvm::Value* Emitter::emitVarInstanceExpr(const VarInstanceExpr* vie) {
  std::vector<llvm::StringMap<llvm::Value*>>::reverse_iterator cur = scopedValues.rbegin();
  std::vector<llvm::StringMap<llvm::Value*>>::reverse_iterator end = scopedValues.rend();
  while(cur != end) {
    llvm::StringMap<llvm::Value*>::iterator e = cur->find(*vie->getName());
    if(e != cur->end()) {
      return e->getValue();
    }
  }
  // find in globals
  fatalError("Variable not found in scope in emitter");
}

llvm::Value* Emitter::emitLiteralExpr(const LiteralExpr* le) {
  llvm::Constant* con = nullptr;
  switch(le->getKind()) {
  case LK_NUM:
    if(le->getLiteral().find('.') != std::string::npos) {
      // this is a floating point constant
      con = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), le->getLiteral());
      return con;
    }
    else {
      // this is an integer constant
      con = llvm::ConstantInt::getSigned(llvm::IntegerType::get(context, 64),
					 std::stol(le->getLiteral()));
      return con;
    }
  case LK_CHAR:
    con = llvm::ConstantInt::getSigned(llvm::IntegerType::get(context, 8), le->getLiteral()[0]);
    return con;
  case LK_STR:
    // ignoring this until string class is created
  default:
    fatalError("Missing case in getLiteral in emitter");
  }
}

llvm::Value* Emitter::removeRefs(llvm::Value* val) {
  if(val->getType()->isPointerTy()) {
    return builder.CreateLoad(val);
  }
  return val;
}

std::pair<llvm::Value*, llvm::Value*> Emitter::readyBOE(const BinaryOperationExpr* boe,
							llvm::Value* l, llvm::Value* r) {
  if(dynamic_cast<const VarInstanceExpr*>(boe->getLeftOperand())) {
    l = builder.CreateLoad(l);
  }
  if(dynamic_cast<const VarInstanceExpr*>(boe->getRightOperand())) {
    r = builder.CreateLoad(r);
  }
  
  llvm::Type* lt = l->getType();
  llvm::Type* rt = r->getType();
  if(lt != rt) {
    // casting of some form is required
    if(lt->isIntegerTy() && rt->isIntegerTy()) {
      llvm::IntegerType* lit = static_cast<llvm::IntegerType*>(lt);
      llvm::IntegerType* rit = static_cast<llvm::IntegerType*>(rt);
      if(lit->getBitWidth() > rit->getBitWidth()) {
	r = builder.CreateSExt(r, lit);
	return std::make_pair(l, r);
      }
      else {
	l = builder.CreateSExt(l, rit);
	return std::make_pair(l, r);
      }
    }
    // other types of casts go here... I know... I'm lazy
  }
  
  std::make_pair(l, r); // no casting needed
}

llvm::Value* Emitter::castType(llvm::Value* val, llvm::Type* t) {
  if(t->isIntegerTy()){
    return castInteger(val, static_cast<llvm::IntegerType*>(t));
  }
}

llvm::Value* Emitter::castInteger(llvm::Value* val, llvm::IntegerType* t) {
  return builder.CreateSExtOrTrunc(val, t);
}

llvm::Type* Emitter::findType(const std::string& name) {
  llvm::StringMap<llvm::StructType*>::iterator clas = classMap.find(name);
  if(clas != classMap.end()) {
    return clas->getValue();
  }
  llvm::StringMap<llvm::Type*>::iterator prim = primitiveMap.find(name);
  if(prim != primitiveMap.end()) {
    return prim->getValue();
  }
  // and so on
}

void Emitter::fatalError(const std::string& errstr) {
  std::cout << errstr;
  exit(-1);
}

void Emitter::emit() {
  emitTop();
  return;
}

std::unique_ptr<llvm::Module>&& Emitter::stripModule() {
  return std::move(module);
}
