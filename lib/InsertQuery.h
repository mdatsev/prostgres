#pragma once

class InsertQuery {
 public:
  std::string table_name;
  std::vector<std::string> fields;
  std::vector<std::string> values;
  static InsertQuery parse(std::string input);

 private:
  InsertQuery(std::string input);
};
