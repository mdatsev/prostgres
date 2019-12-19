#pragma once
#include <string_view>
#include <vector>
#include "DBTypes.hpp"
#include "ParserHelper.hpp"

class InsertQuery {
 public:
  std::string_view table_name;
  std::vector<std::string_view> fields;
  std::vector<std::string_view> values;
  
  static InsertQuery parse (std::string_view input) {
    return InsertQuery(input);
  }
 private:
  InsertQuery (std::string_view input) {
    ParserHelper ph(input);
    ph.get_word(); // INSERT
    ph.get_word(); // INTO
    table_name = ph.get_word();
    assertUser(table_name.size() != 0, "Table name invalid");
    assertUser(ph.get_token() == "(", "No columns specified");
    while (true) {
      auto col_name = ph.get_word();
      auto next_tok = ph.get_token();
      fields.push_back(col_name);
      if(next_tok == ")") {
        break;
      }
    }
    ph.get_token(); // )
    ph.get_word(); // VALUES
    ph.get_token(); // (
    while (true) {
      auto literal = ph.get_literal();
      if (literal.size() == 0) {
        break;
      }
      ph.get_token(); // ,
      values.push_back(literal);
    }
    
  }
};