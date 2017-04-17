#ifndef QUAL_TYPE_HPP
#define QUAL_TYPE_HPP

#include "ast_shared.hpp"
class Type;

class QualType {
private:
  Type* type;
  // [const, static]
  std::vector<bool> typeBits;
public:
  QualType() = delete;
  QualType(Type* t) : type(t), typeBits(2) {}
  virtual ~QualType() = default;
  
  Type* getType() { return type; }
  const Type* getType() const { return type; }
  void setType(Type* t) { type = t; }
  
  bool getConst() const { return typeBits[0]; }
  bool getStatic() const { return typeBits[1]; }
  
  void setConst(bool b) { typeBits[0] = b; }
  void setStatic(bool b) { typeBits[1] = b; }
};

#include "type.hpp"

#endif
