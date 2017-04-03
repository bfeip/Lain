#ifndef MODULE_HPP
#define MODULE_HPP

#include "ast_shared.hpp"

#include "stmt.hpp"

#include "scope_creator.hpp"
#include "function_decl.hpp"

#include "null_stmt.hpp"
#include "if_stmt.hpp"
#include "else_stmt.hpp"
#include "while_stmt.hpp"
#include "for_stmt.hpp"
#include "switch_stmt.hpp"
#include "break_stmt.hpp"
#include "continue_stmt.hpp"
#include "return_stmt.hpp"
#include "var_decl_stmt.hpp"

#include "literal_kind.hpp"
#include "literal_expr.hpp"
#include "var_instance_expr.hpp"
#include "function_call_expr.hpp"
#include "unary_operation_expr.hpp"
#include "binary_operation_expr.hpp"
#include "grouped_expr.hpp"

#include "type.hpp"
#include "primitive_type.hpp"
#include "scope_tree.hpp"

class Module : virtual public ScopeCreator {
private:
  std::string name;
  std::vector<std::unique_ptr<VarDecl>> globals;
  std::vector<std::string> usings;
  llvm::StringMap<std::unique_ptr<Module>>& externals;
public:
  Module(const std::string& name, llvm::StringMap<std::unique_ptr<Module>>& ex) :
    ScopeCreator(this), name(name), externals(ex) {
    std::unique_ptr<Type> Void(new Type("void", nullptr));
    std::unique_ptr<Type> Bool(new Type("bool", nullptr));
    std::unique_ptr<Type> Byte(new Type("byte", nullptr));
    std::unique_ptr<Type> UByte(new Type("ubyte", nullptr));
    std::unique_ptr<Type> Short(new Type("short", nullptr));
    std::unique_ptr<Type> UShort(new Type("ushort", nullptr));
    std::unique_ptr<Type> Int(new Type("int", nullptr));
    std::unique_ptr<Type> UInt(new Type("uint", nullptr));
    std::unique_ptr<Type> Long(new Type("long", nullptr));
    std::unique_ptr<Type> ULong(new Type("ulong", nullptr));
    std::unique_ptr<Type> LongLong(new Type("longlong", nullptr));
    std::unique_ptr<Type> ULongLong(new Type("ulonglong", nullptr));
    std::unique_ptr<Type> Float(new Type("float", nullptr));
    std::unique_ptr<Type> Double(new Type("double", nullptr));
    std::unique_ptr<Type> LongDouble(new Type("longdouble", nullptr));
    std::unique_ptr<Type> String(new Type("string", nullptr));
    addOwnedType(std::move(Void), AM_VOID);
    addOwnedType(std::move(Bool), AM_VOID);
    addOwnedType(std::move(Byte), AM_VOID);
    addOwnedType(std::move(UByte), AM_VOID);
    addOwnedType(std::move(Short), AM_VOID);
    addOwnedType(std::move(UShort), AM_VOID);
    addOwnedType(std::move(Int), AM_VOID);
    addOwnedType(std::move(UInt), AM_VOID);
    addOwnedType(std::move(Long), AM_VOID);
    addOwnedType(std::move(ULong), AM_VOID);
    addOwnedType(std::move(LongLong), AM_VOID);
    addOwnedType(std::move(ULongLong), AM_VOID);
    addOwnedType(std::move(Float), AM_VOID);
    addOwnedType(std::move(Double), AM_VOID);
    addOwnedType(std::move(LongDouble), AM_VOID);
    addOwnedType(std::move(String), AM_VOID);
  }

  const std::string& getName() const { return name; }
  void setName(const std::string& str) { name = str; }

  void addGlobal(std::unique_ptr<VarDecl> global);
  const std::vector<const VarDecl*> getGlobals() const {
    std::vector<const VarDecl*> ret;
    for(const std::unique_ptr<VarDecl>& global : globals) {
      ret.push_back(global.get());
    }
    return ret;
  }

  void addUsing(const std::string& fn) { usings.push_back(fn); }
  std::vector<std::string>& getUsings() { return usings; }
  const std::vector<std::string>& getUsings() const { return usings; }

  llvm::StringMap<std::unique_ptr<Module>>& getExternals() { return externals; }
  const llvm::StringMap<std::unique_ptr<Module>>& getExternals() const { return externals; }

  void resolveTypes();
};

#endif
