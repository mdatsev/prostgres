#include <iostream>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

using table_id = std::int64_t;
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

void throwUser (const char* message) {
  throw UserError(message);
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
  ParserHelper(const std::string_view& input, const std::size_t curr_pos = 0)
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

 private:
  const std::string_view& input;
  std::size_t curr_pos;
};

class CreateQuery {
 public:
  std::string_view table_name;
  
  static CreateQuery parse (const std::string_view& input) {
    return CreateQuery(input);
  }
 private:
  CreateQuery (const std::string_view& input) {
    ParserHelper ph(input);
    ph.get_word(); // CREATE
    auto object = ph.get_word(); // TABLE
    auto name = ph.get_word();
    assertUser(name.size() != 0, "Table name invalid");
    table_name = name;
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
    std::cout << "created table " << q.table_name << ";" << std::endl;
  }

  table_id create_table (const std::string_view name) {
    const auto id = get_unique_table_id();
    const auto table_dir = tables_dir / std::to_string(id);
    fs::create_directory(table_dir);
    return Table::create(id, table_dir).id;
  }

 private:
  table_id table_id_counter = 0;
  table_id get_unique_table_id () {
    table_id_counter++;
    return table_id_counter;
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
      std::string query_string;

      std::getline(std::cin, query_string, ';');

      auto command = ParserHelper(query_string).get_word();
      if (command == "create") {
        db.execute(CreateQuery::parse(query_string));
      } else {
        throwUser("Invalid command");
      }
    } catch (UserError e) {
      std::cout << e.what() << ". Please check the manual :)\n" << std::endl;
    }
  }

  std::cout << db.create_table("users");
}