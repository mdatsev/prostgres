#pragma once

class SelectQuery {
 public:
  std::string table_name;
  std::vector<std::string> fields;
  static SelectQuery parse(std::string input);

 private:
  SelectQuery(std::string input);
};