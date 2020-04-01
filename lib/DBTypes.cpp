#include <inttypes.h>
#include <stdlib.h>
#include <string>
#include <variant>

#include "DBTypes.h"
#include "errors.h"

DBType toDBType(std::string type_name) {
  if (type_name == "string") {
    return DBType::string;
  } else if (type_name == "int" || type_name == "int64") {
    return DBType::int64;
  } else {
    throw UserError("Invalid type");
  }
}
DBValue parseIntLiteral(std::string literal) { return DBValue(std::in_place_index<(int)DBType::int64>, atoi(literal.c_str())); }
std::string toString(DBType type) {
  if (type == DBType::string) {
    return "string";
  } else if (type == DBType::int64) {
    return "int64";
  } else {
    throw UserError("Invalid type while decoding");
  }
}
Field::Field(DBType type, std::string name) : type(type), name(name) {}

Op parseOp(std::string str) {
  if (str == "<=") return Op::le;
  if (str == ">=") return Op::ge;
  if (str == "<" ) return Op::lt;
  if (str == ">" ) return Op::gt;
  if (str == "==") return Op::eq;
  if (str == "!=") return Op::ne;
  throw UserError("Invalid operator");
}

bool apply_op(INT64_type lhs, Op op, INT64_type rhs) {
  if (op == Op::le) return lhs <= rhs;
  if (op == Op::ge) return lhs >= rhs;
  if (op == Op::lt) return lhs <  rhs;
  if (op == Op::gt) return lhs >  rhs;
  if (op == Op::eq) return lhs == rhs;
  if (op == Op::ne) return lhs != rhs;
}