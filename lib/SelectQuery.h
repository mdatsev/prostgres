#pragma once
#include "DBTypes.h"
struct Condition {
  std::string column;
  Op op;
  std::string value;
};

class SelectQuery {
 public:
  std::string table_name;
  std::vector<std::string> fields;
  static SelectQuery parse(std::string input);
  bool conditional;
  Condition condition;

 private:
  SelectQuery(std::string input);
};