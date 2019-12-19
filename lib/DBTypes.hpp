#pragma once
#include <inttypes.h>
#include <string_view>
#include <stdlib.h>
#include "errors.hpp"

using table_id = std::int64_t;
using field_id = std::int64_t;

enum DBType : int64_t {
  INT64 = 1,
  STRING = 2
};

union DBValue {
  int64_t int64;
  std::string_view str;
};

DBType toDBType(std::string_view type_name) {
  if (type_name == "string") {
    return DBType::STRING;
  } else if (type_name == "int" || type_name == "int64") {
    return DBType::INT64;
  } else {
    throw UserError("Invalid type");
  }
}

DBValue parseIntLiteral(std::string_view literal) {
  return {atoi(literal.data())};
}

std::string_view toString(DBType type) {
  if (type == DBType::STRING) {
    return "string";
  } else if (type == DBType::INT64) {
    return "int64";
  } else {
    throw UserError("Invalid type while decoding");
  }
}

class Field {
 public:
  DBType type;
  std::string_view name;

  Field(DBType type, std::string_view name)
    : type(type), name(name) {}
};