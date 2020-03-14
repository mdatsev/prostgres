#pragma once

#include <variant>

using table_id = std::int64_t;
using field_id = std::int64_t;
using meta_int = std::int64_t;

using INT64_type = int64_t;
using STRING_type = std::string;
enum class DBType : meta_int {
  int64 = 0,
  string = 1,
  table_id = 2
}; /* ^ must be the same order V */
using DBValue = std::variant<INT64_type, STRING_type, table_id>;

DBType toDBType(std::string type_name);
DBValue parseIntLiteral(std::string literal);
std::string toString(DBType type);
class Field {
 public:
  DBType type;
  std::string name;
  Field(DBType type, std::string name);
};

enum class Op {
  le,
  ge,
  lt,
  gt,
  eq,
  ne
};
Op parseOp(std::string str);
bool use_op(INT64_type lhs, Op op, INT64_type rhs);