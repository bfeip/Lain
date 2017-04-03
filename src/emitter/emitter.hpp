#ifndef EMITTER_HPP
#define EMITTER_HPP

#include <iostream>
#include "llvm_includes.hpp"
#include "parser.hpp"

class Emitter {
private:
  const Module* tu;
  
  llvm::LLVMContext& context;
  llvm::IRBuilder<> builder;
  std::unique_ptr<llvm::Module> module;

  // type maps
  llvm::StringMap<llvm::Type*> primitiveMap;
  llvm::StringMap<llvm::StructType*> classMap;
  llvm::StringMap<llvm::FunctionType*> functionTypeMap;

  std::stack<llvm::Function*> functionStack;
  std::stack<llvm::BasicBlock*> breakTo;
  std::stack<llvm::BasicBlock*> continueTo;

  std::vector<std::unique_ptr<llvm::GlobalVariable>> globalVars;
  std::vector<llvm::StringMap<llvm::Value*>> scopedValues;

  std::vector<llvm::Instruction*> delayedInstructions;
  
  void emitTop();

  void emitGlobalVar(const VarDecl* vd);

  void emitType(const TypeDecl* td);
  
  void emitClass(const ClassDecl* cd);
  //void emitEnum(const EnumDecl* ed); TODO
  void emitFunctionDec(const FunctionDecl* fd);
  void emitFunctionDef(const FunctionDecl* fd);
  void emitMethod(const FunctionDecl* fd, llvm::StructType* self);

  void emitStmt(const Stmt* stmt);
  void emitCompoundStmt(const CompoundStmt* cs);
  void emitVarDeclStmt(const VarDeclStmt* vds);
  void emitIfStmt(const IfStmt* is, llvm::BasicBlock* exit = nullptr);
  void emitWhileStmt(const WhileStmt* ws);
  void emitForStmt(const ForStmt* fs);
  //void emitSwitchStmt(const SwitchStmt* ss); TODO
  void emitBreakStmt(const BreakStmt* bs);
  void emitContinueStmt(const ContinueStmt* cs);
  void emitReturnStmt(const ReturnStmt* rs);
  
  llvm::Value* emitExpr(const Expr* e);
  llvm::Value* emitBinaryOperationExpr(const BinaryOperationExpr* boe);
  llvm::Value* emitUnaryOperationExpr(const UnaryOperationExpr* uoe);
  llvm::Value* emitFunctionCallExpr(const FunctionCallExpr* fce);
  llvm::Value* emitVarInstanceExpr(const VarInstanceExpr* vie);
  llvm::Value* emitLiteralExpr(const LiteralExpr* le);

  llvm::Value* removeRefs(llvm::Value* val);
  
  // Expr support stuff
  std::pair<llvm::Value*, llvm::Value*> readyBOE(const BinaryOperationExpr* boe,
						 llvm::Value* l, llvm::Value* r);
  llvm::Value* castType(llvm::Value* val, llvm::Type* t);
  llvm::Value* castInteger(llvm::Value* val, llvm::IntegerType* t);

  llvm::Type* findType(const std::string& name);
  void fatalError(const std::string& errstr);
public:
  Emitter() = delete;
  Emitter(llvm::LLVMContext& c, const Module* tu) :
    tu(tu), context(c), builder(c),
    module(new llvm::Module(tu->getName(), c)) {
    
    primitiveMap.insert(std::make_pair("void", llvm::Type::getVoidTy(context)));
    primitiveMap.insert(std::make_pair("bool", llvm::Type::getIntNTy(context, 1)));
    primitiveMap.insert(std::make_pair("byte", llvm::Type::getIntNTy(context, 8)));
    primitiveMap.insert(std::make_pair("ubyte", llvm::Type::getIntNTy(context, 8)));
    primitiveMap.insert(std::make_pair("short", llvm::Type::getIntNTy(context, 16)));
    primitiveMap.insert(std::make_pair("ushort", llvm::Type::getIntNTy(context, 16)));
    primitiveMap.insert(std::make_pair("int", llvm::Type::getIntNTy(context, 32)));
    primitiveMap.insert(std::make_pair("uint", llvm::Type::getIntNTy(context, 32)));
    primitiveMap.insert(std::make_pair("long", llvm::Type::getIntNTy(context, 64)));
    primitiveMap.insert(std::make_pair("ulong", llvm::Type::getIntNTy(context, 64)));
    primitiveMap.insert(std::make_pair("longlong", llvm::Type::getIntNTy(context, 124)));
    primitiveMap.insert(std::make_pair("ulonglong", llvm::Type::getIntNTy(context, 124)));
    primitiveMap.insert(std::make_pair("float", llvm::Type::getFloatTy(context)));
    primitiveMap.insert(std::make_pair("double", llvm::Type::getDoubleTy(context)));
    primitiveMap.insert(std::make_pair("longdouble", llvm::Type::getFP128Ty(context)));
  }

  void emit();
  std::unique_ptr<llvm::Module>&& stripModule();
};

#endif
