#pragma once

#include <variant>

using table_id = std::int64_t;
using field_id = std::int64_t;
using meta_int = std::int64_t;

using INT64_type = int64_t;
using STRING_type = std::string;
enum DBType : meta_int {
  INT64 = 0,
  STRING = 1
}; /* ^ must be the same order V */
using DBValue = std::variant<INT64_type, STRING_type>;

DBType toDBType(std::string type_name);
DBValue parseIntLiteral(std::string literal);
std::string toString(DBType type);
class Field {
 public:
  DBType type;
  std::string name;
  Field(DBType type, std::string name);
};