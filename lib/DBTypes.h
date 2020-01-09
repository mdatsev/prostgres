#pragma once

#include <variant>

using table_id = std::int64_t;
using field_id = std::int64_t;
using DBValue = std::variant<int64_t, std::string>;

enum DBType : int64_t {
  INT64 = 1,
  STRING = 2
};


DBType toDBType (std::string type_name);
DBValue parseIntLiteral (std::string literal);
std::string toString (DBType type);
class Field
{
public:
  DBType type;
  std::string name;
  Field (DBType type, std::string name);
};