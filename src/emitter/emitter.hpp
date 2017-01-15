#ifndef EMITTER_HPP
#define EMITTER_HPP

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
  llvm::StringMap<llvm::Value*> globalValues;
  std::vector<llvm::StringMap<llvm::Value*>> scopedValues;
  
  void emitTop();

  void emitType();
  
  void emitClass(const ClassDecl* cd);
  void emitEnum(const EnumDecl* ed);
  void emitFunction(const FunctionDecl* fd);
  void emitMethod(const FunctionDecl* fd, const llvm::StructType* self);

  void emitStmt(const Stmt* stmt);
  void emitCompoundStmt(const CompoundStmt* cs);
  void emitVarDeclStmt(const VarDeclStmt* vds);
  void emitIfStmt(const IfStmt* is);
  void emitWhileStmt(const WhileStmt* ws);
  void emitForStmt(const ForStmt* fs);
  void emitSwitchStmt(const SwitchStmt* ss);
  void emitBreakStmt(const BreakStmt* bs);
  void emitContinueStmt(const ContinueStmt* cs);
  void emitReturnStmt(const ReturnStmt* rs);
  
  llvm::Value* emitExpr(const Expr* e);
  std::pair<llvm::Value*, std::vector<llvm::Inst*>>
    emitExprMain(const Expr* e);
  std::pair<llvm::Value*, std::vector<llvm::Inst*>>
    emitBinaryOperationExpr(const BinaryOperationExpr* boe);
  std::pair<llvm::Value*, std::vector<llvm::Inst*>>
    emitUnaryOperationExpr(const UnaryOperationExpr* uoe);
  std::pair<llvm::Value*, std::vector<llvm::Inst*>>
    emitFunctionCallExpr(const FunctionCallExpr* fce);
  std::pair<llvm::Value*, std::vector<llvm::Inst*>>
    emitVariableInstanceExpr(const VariableInstanceExpr* vie);
  std::pair<llvm::Value*, std::vector<llvm::Inst*>>
    emitLiteralExpr(const LiteralExpr* le);

  llvm::Type* findType(const std::string& name);
  void fatalError(const std::string& errstr);
public:
  Emitter() = delete;
  Emitter(std::unique_ptr<Module> m) :
    tu(std::move(m)), context(), builder(context),
    module(new llvm::Module(m->getName())) {
    // insert primitive type mappings
  }

  void emit(const std::string* filename);
};

#endif
