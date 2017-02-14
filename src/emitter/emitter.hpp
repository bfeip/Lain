#ifndef EMITTER_HPP
#define EMITTER_HPP

#include <iostream>
#include "llvm_includes.hpp"
#include "parser.hpp"

class Emitter {
private:
  const std::unique_ptr<const Module> tu;
  
  llvm::LLVMContext context;
  llvm::IRBuilder<> builder;
  std::unique_ptr<llvm::Module> module;

  // type maps
  std::map<PrimitiveType, llvm::Type*> primitiveMap;
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
  void emitFunction(const FunctionDecl* fd);
  void emitMethod(const FunctionDecl* fd, llvm::StructType* self);

  void emitStmt(const Stmt* stmt);
  void emitCompoundStmt(const CompoundStmt* cs);
  void emitVarDeclStmt(const VarDeclStmt* vds);
  void emitIfStmt(const IfStmt* is);
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

  llvm::Type* findType(const std::string& name);
  llvm::Value* findValue(const std::string& name);
  void fatalError(const std::string& errstr);
public:
  Emitter() = delete;
  Emitter(std::unique_ptr<Module> m) :
    tu(std::move(m)), context(), builder(context),
    module(new llvm::Module(m->getName(), context)) {
    // insert primitive type mappings
  }

  void emit();
};

#endif
