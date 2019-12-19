#pragma once
#include <string_view>
#include <vector>
#include "DBTypes.hpp"
#include "ParserHelper.hpp"

class SelectQuery {
 public:
  std::string_view table_name;
  std::vector<std::string_view> fields;
  
  static SelectQuery parse (std::string_view input) {
    return SelectQuery(input);
  }
 private:
  SelectQuery (std::string_view input) {
    ParserHelper ph(input);
    ph.get_word(); // SELECT
    while (true) {
      auto col_name = ph.get_word();
      auto next_tok = ph.get_token();
      fields.push_back(col_name);
      if(next_tok.size() == 0) {
        break;
      }
    }
    ph.get_word(); // FROM
    table_name = ph.get_word();
  }
};