#include <string>
#include <vector>

#include "SelectQuery.h"

#include "ParserHelper.h"

SelectQuery SelectQuery::parse (std::string input)
                                               {
    return SelectQuery(input);
  }
SelectQuery::SelectQuery (std::string input)
                                  {
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
#undef LZZ_INLINE
