#include <string>
#include <vector>

#include "InsertQuery.h"

#include "ParserHelper.h"
#include "errors.h"

InsertQuery InsertQuery::parse(std::string input) {
  return InsertQuery(input);
}
InsertQuery::InsertQuery(std::string input) {
  ParserHelper ph(input);
  ph.get_word();  // INSERT
  ph.get_word();  // INTO
  table_name = ph.get_word();
  assertUser(table_name.size() != 0, "Table name invalid");
  assertUser(ph.get_token() == "(", "No columns specified");
  while (true) {
    auto col_name = ph.get_word();
    auto next_tok = ph.get_token();
    fields.push_back(col_name);
    if (next_tok == ")") {
      break;
    }
  }
  ph.get_token();  // )
  ph.get_word();   // VALUES
  ph.get_token();  // (
  while (true) {
    auto literal = ph.get_literal();
    if (literal.size() == 0) {
      break;
    }
    values.push_back(literal);
    if (ph.get_token() == ")") {
      break;
    };  // ,
  }
}