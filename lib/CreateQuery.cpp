#include <string>
#include <vector>

#include "CreateQuery.h"
#include "DBTypes.h"
#include "ParserHelper.h"
#include "errors.h"

CreateQuery CreateQuery::parse(std::string input) { return CreateQuery(input); }

CreateQuery::CreateQuery(std::string input) {
  ParserHelper ph(input);
  ph.get_word();                // CREATE
  auto object = ph.get_word();  // TABLE
  table_name = ph.get_word();
  assertUser(table_name.size() != 0, "Table name invalid");
  ph.get_token();  // (
  while (true) {
    auto col_name = ph.get_word();
    auto type = ph.get_word();
    auto next_tok = ph.get_token();
    fields.push_back({toDBType(type), col_name});
    if (next_tok == ")") {
      break;
    }
  }
}