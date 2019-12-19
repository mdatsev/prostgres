#pragma once
#include <string_view>
#include <vector>
#include "DBTypes.hpp"
#include "ParserHelper.hpp"

class CreateQuery {
 public:
  std::string_view table_name;
  std::vector<Field> fields;
  
  static CreateQuery parse (std::string_view input) {
    return CreateQuery(input);
  }
 private:
  CreateQuery (std::string_view input) {
    ParserHelper ph(input);
    ph.get_word(); // CREATE
    auto object = ph.get_word(); // TABLE
    table_name = ph.get_word();
    assertUser(table_name.size() != 0, "Table name invalid");
    ph.get_token(); // (
    while (true) {
      auto col_name = ph.get_word();
      auto type = ph.get_word();
      auto next_tok = ph.get_token();
      fields.push_back({toDBType(type), col_name});
      if(next_tok == ")") {
        break;
      }
    }
  }
};