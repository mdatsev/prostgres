#include <inttypes.h>
#include <string>
#include <stdlib.h>
#include <variant>

#include "DBTypes.h"
#include "errors.h"

DBType toDBType (std::string type_name)
                                       {
  if (type_name == "string") {
    return DBType::STRING;
  } else if (type_name == "int" || type_name == "int64") {
    return DBType::INT64;
  } else {
    throw UserError("Invalid type");
  }
}
DBValue parseIntLiteral (std::string literal)
                                             {
  return {atoi(literal.c_str())};
}
std::string toString (DBType type)
                                  {
  if (type == DBType::STRING) {
    return "string";
  } else if (type == DBType::INT64) {
    return "int64";
  } else {
    throw UserError("Invalid type while decoding");
  }
}
Field::Field (DBType type, std::string name)
  : type (type), name (name)
                             {}
