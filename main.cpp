#include <iostream>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

using table_id = std::int64_t;
using field_id = std::int64_t;
constexpr const char DELIM = ';';

class UserError : std::logic_error {
 public:
  using std::logic_error::logic_error;
  using std::logic_error::what;
};

void assertUser (const bool cond, const char* message) {
  if (!cond) {
    throw UserError(message);
  }
}

class Table {
 public:
  const table_id id;

  static Table create (const table_id id, const fs::path files_dir) {
    Table table = Table{id, files_dir};
    table.create();
    return table;
  }

  static Table load (const table_id id, const fs::path files_dir) {
    Table table = Table{id, files_dir};
    table.load();
    return table;
  }

 private:
  const fs::path data_path;
  const fs::path page_map_path;
  const fs::path toast_path;
  const fs::path meta_path;

  Table (const table_id id, const fs::path files_dir)
    : id { id },
      data_path     { files_dir / "data"     },
      page_map_path { files_dir / "page_map" },
      toast_path    { files_dir / "toast"    },
      meta_path     { files_dir / "meta"     } {
  }

  void create() {
    auto mode = std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary;

    std::fstream meta_file;
    meta_file.open(meta_path, mode);

    std::fstream data_file;
    data_file.open(data_path, mode);

    std::fstream page_map_file;
    page_map_file.open(page_map_path, mode);

    std::fstream toast_file;
    toast_file.open(toast_path, mode);
  }

  void load() {

  }
};

bool is_word_char (char c) {
  return (c >= 'a' && c <= 'z') ||
          (c >= 'A' && c <= 'Z') ||
          (c >= '0' && c <= '9') ||
          c == '_';
}

bool is_digit (char c) {
  return (c >= '0' && c <= '9');
}

bool is_word_begin_char (char c) {
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
          c == '_';
}

bool is_token_char (char c) {
  return c == '(' || c == ',' || c == ')';
}

bool is_whitespace (char c) {
  return c == ' ' || c == '\t' || c == '\n';
}

bool is_delim (char c) {
  return c == ';';
}

class ParserHelper {
 public:
  ParserHelper(std::string_view input, const std::size_t curr_pos = 0)
    : input(input), curr_pos(curr_pos) {}

  std::string_view get_word() {
    std::size_t start = std::string::npos;
    std::size_t end = std::string::npos;
    for (auto i = curr_pos; i < input.size(); i++) {
      if (start == std::string::npos) {
        // std::cout << "BEGIN?:" << input[i] << std::endl;
        if (is_word_begin_char(input[i])) {
          // std::cout << "BEGIN:" << input[i] << std::endl;
          start = i;
        } else if (is_whitespace(input[i])) {
          continue;
        } else {
          break;
        }
      } else {
        // std::cout << "END?:" << input[i] << std::endl;
        if (!is_word_char(input[i])) {
          // std::cout << "END:" << input[i] << std::endl;
          end = i;
          break;
        }
      }
    }
    if (start == std::string::npos) {
      // fail to find
      return {};
    }

    curr_pos = end;
    return input.substr(start, end - start);
  }
  
  std::string_view get_token() {
    for (auto i = curr_pos; i < input.size() && !is_delim(input[i]); i++) {
      if (is_token_char(input[i])) {
        curr_pos = i + 1;
        return input.substr(i, 1);
      } else if (is_whitespace(input[i])) {
        continue;
      } else {
        break;
      }
    }
    // fail to find
    return {};
  }

  std::string_view get_int_literal() {
    std::size_t start = std::string::npos;
    std::size_t end = std::string::npos;
    for (auto i = curr_pos; i < input.size(); i++) {
      if (start == std::string::npos) {
        if (is_digit(input[i])) {
          start = i;
        } else if (is_whitespace(input[i])) {
          continue;
        } else {
          break;
        }
      } else {
        if (!is_digit(input[i])) {
          end = i;
          break;
        }
      }
    }
    if (start == std::string::npos) {
      // fail to find
      return {};
    }

    curr_pos = end;
    return input.substr(start, end - start);
  }

 private:
  std::string_view input;
  std::size_t curr_pos;
};

enum DBType : int64_t {
  INT64 = 1,
  STRING = 2
};

union DBValue {
  int64_t int64;
  std::string_view str;
};

DBType toDBType(std::string_view type_name) {
  if (type_name == "string") {
    return DBType::STRING;
  } else if (type_name == "int" || type_name == "int64") {
    return DBType::INT64;
  } else {
    throw UserError("Invalid type");
  }
}

DBValue parseIntLiteral(std::string_view literal) {
  return {atoi(literal.data())};
}

std::string_view toString(DBType type) {
  if (type == DBType::STRING) {
    return "string";
  } else if (type == DBType::INT64) {
    return "int64";
  } else {
    throw UserError("Invalid type while decoding");
  }
}

class Field {
 public:
  DBType type;
  std::string_view name;

  Field(DBType type, std::string_view name)
    : type(type), name(name) {}
};

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

class InsertQuery {
 public:
  std::string_view table_name;
  std::vector<std::string_view> fields;
  std::vector<DBValue> values;
  
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
    ph.get_token(); // (
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
      auto literal = ph.get_int_literal();
      if (literal.size() == 0) {
        break;
      }
      ph.get_token(); // ,
      values.push_back(parseIntLiteral(literal));
    }
    
  }
};

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

class DB {
 public:
  const fs::path base_dir;
  const fs::path tables_dir;
  
  DB (fs::path base_dir)
    : base_dir { base_dir },
      tables_dir { base_dir / "tables" } {
    initialize_file_structure(base_dir);
  }

  void execute (CreateQuery q) {
    std::cout << "created table " << q.table_name << "(";
    for(auto field : q.fields) {
      std::cout << field.name << " " << toString(field.type) << ", ";
    }
    std::cout << ");\n" << std::endl;
  }

  void execute (InsertQuery q) {
    std::cout << "inserted into table " << q.table_name << "(";
    for(auto field : q.fields) {
      std::cout << field << ", ";
    }
    std::cout << ") values (";
    for(auto value : q.values) {
      std::cout << value.int64 << ", ";
    }
    std::cout << ");\n" << std::endl;
  }

  void execute (SelectQuery q) {
    std::cout << "selected ";
    for(auto field : q.fields) {
      std::cout << field << ", ";
    }
    std::cout << "from " << q.table_name << ";\n";
  }

  table_id create_table (std::string_view name) {
    const auto id = get_unique_table_id();
    const auto table_dir = tables_dir / std::to_string(id);
    fs::create_directory(table_dir);
    return Table::create(id, table_dir).id;
  }

 private:
  int64_t unique_id_counter = 0;
  table_id get_unique_table_id () {
    return ++unique_id_counter;
  }

  field_id get_unique_field_id () {
    return ++unique_id_counter;
  }

  void initialize_file_structure (const fs::path base_dir) {
    fs::create_directory(base_dir);
    fs::create_directory(tables_dir);
  }
};

int main (int argc, const char** argv) {
  DB db("./db");
  
  while (std::cin) {
    try {
      std::cout << "> "; 
      std::string buffer;
      const std::string& query_string = buffer;

      std::getline(std::cin, buffer, ';');
      if(std::cin.eof()) {
        break;
      }
      auto command = ParserHelper(query_string).get_word();
      if (command == "create") {
        db.execute(CreateQuery::parse(query_string));
      } else if (command == "insert") {
        db.execute(InsertQuery::parse(query_string));
      } else if (command == "select") {
        db.execute(SelectQuery::parse(query_string));
      } else {
        throw UserError("Invalid command");
      }
    } catch (UserError e) {
      std::cout << e.what() << ". Please check the manual :)\n" << std::endl;
    }
  }
}