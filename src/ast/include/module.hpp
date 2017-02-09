#ifndef MODULE_HPP
#define MODULE_HPP

#include "ast_shared.hpp"

#include "stmt.hpp"

#include "scope_creator.hpp"
#include "type_decl.hpp"
#include "typedef_decl.hpp"
#include "class_decl.hpp"
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

#include "scope_tree.hpp"

class Module : virtual public ScopeCreator {
private:
  ScopeTree types; /* tree of TU types, a node can only "see" its immediate children, 
		    * its siblings, any of its parent nodes, or any top level node */
  std::vector<std::unique_ptr<VarDecl>> globals;
public:
  Module() : ScopeCreator(this) {}

  void addGlobal(std::unique_ptr<VarDecl> global);
  
  void resolveTypes();
};

#endif
