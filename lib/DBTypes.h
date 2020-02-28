#pragma once

#include <variant>

using table_id = std::int64_t;
using field_id = std::int64_t;

enum DBType : int64_t {
  INT64 = 0,
  STRING = 1
}; /* must be the same order */ using DBValue = std::variant<int64_t, std::string>;

using INT64_type = int64_t;
using STRING_type = std::string ;

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

using meta_int = int64_t;