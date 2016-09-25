#ifndef QUAL_TYPE_HPP
#define QUAL_TYPE_HPP

#include "ast_shared.hpp"
#include "type.hpp"

class QualType {
private:
  Type type;
  // [const, static]
  std::vector<bool> typeBits;
public:
  QualType() : typeBits(2) {}
  type getType() { return type; }
  void setType(const Type& type) { this->type = type; }
  
  bool getConst() const { return typeBits[0]; }
  bool getStatic() const { return typeBits[1]; }
  
  void setConst(bool b) { typeBits[0] = b; }
  void setStatic(bool b) { typeBits[1] = b; }
};

#endif
