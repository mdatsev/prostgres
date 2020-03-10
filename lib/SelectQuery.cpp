#include <string>
#include <vector>

#include "SelectQuery.h"
#include "errors.h"
#include "ParserHelper.h"

SelectQuery SelectQuery::parse(std::string input) {
  return SelectQuery(input);
}

Op parseOp(std::string str) {
  if (str == "<=") return Op::le;
  if (str == ">=") return Op::ge;
  if (str == "<" ) return Op::lt;
  if (str == ">" ) return Op::gt;
  if (str == "==") return Op::eq;
  if (str == "!=") return Op::ne;
  throw UserError("Invalid operator");
}

SelectQuery::SelectQuery(std::string input) {
  ParserHelper ph(input);
  ph.get_word();  // SELECT
  while (true) {
    auto col_name = ph.get_word();
    auto next_tok = ph.get_token();
    fields.push_back(col_name);
    if (next_tok.size() == 0) {
      break;
    }
  }
  ph.get_word();  // FROM
  table_name = ph.get_word();
  auto next_clause = ph.get_word(); // WHERE
  if (next_clause.size() != 0) {
    conditional = true;
    // for now always [column] [operator] [value]
    condition.column = ph.get_word();
    condition.op = parseOp(ph.get_token());
    condition.value = ph.get_literal();
  } else {
    conditional = false;
  }
}