#ifndef NULL_STMT_HPP
#define NULL_STMT_HPP

#include "stmt.hpp"

class NullStmt : virtual public Stmt {
public:
  NullStmt() = delete;
  NullStmt(ScopeCreator* p) : Stmt(p) {}
  virtual ~NullStmt() = default;
};

#endif
