#include <fstream>
#include <iostream>
#include <unordered_map>
#include <utility>

#include "DB.h"
#include "Table.h"
#include "errors.h"
#include "serialize.h"

DB::DB(fs::path base_dir)
    : base_dir{base_dir}, tables_dir{base_dir / "tables"} {
  if (fs::create_directory(base_dir)) {
    fs::create_directory(tables_dir);
    unique_id_counter = 0;
    save_global_metadata();
  } else {
    load_global_metadata();
  };
}
void DB::execute(CreateQuery q) {
  if (!silent) {
    std::cout << "created table " << q.table_name << "(";
    for (auto field : q.fields) {
      std::cout << field.name << " " << toString(field.type) << ", ";
    }
    std::cout << ");\n"
              << std::endl;
  }
  table_id id = get_unique_id();
  loaded_tables.emplace(std::piecewise_construct,
                        std::forward_as_tuple(id), 
                        std::forward_as_tuple(id, q.table_name, q.fields, *this));
}
void DB::execute(InsertQuery q) {
  if (!silent) {
    std::cout << "inserted into table " << q.table_name << "(";
    for (auto field : q.fields) {
      std::cout << field << ", ";
    }
    std::cout << ") values (";
    for (auto value : q.values) {
      std::cout << value << ", ";
    }
    std::cout << ");\n"
              << std::endl;
  }
  table_id id = get_table_id(q.table_name);

  auto cache_iter = loaded_tables.find(id);
  if (cache_iter == loaded_tables.end()) {
    std::tie(cache_iter, std::ignore)
      = loaded_tables.emplace(std::piecewise_construct,
                              std::forward_as_tuple(id),
                              std::forward_as_tuple(id, *this));
  }
  Table& t = (*cache_iter).second;
  assertUser(t.nfields == q.values.size(), "Incorrect number of values");
  assertUser(t.nfields == q.fields.size(), "Incorrect number of fields");

  t.insert(q);
}

void print_results(std::vector<std::vector<DBValue>> results) {
  for (auto& row : results) {
    std::cout << "|";
    for (auto& value : row) {
      std::cout << std::to_string(std::get<INT64_type>(value)) << "|";
    }
    std::cout << std::endl;
  }
}

void DB::execute(SelectQuery q) {
  if (!silent) {
    std::cout << "selected ";
    for (auto field : q.fields) {
      std::cout << field << ", ";
    }
    std::cout << "from " << q.table_name << ";\n";
  }
  Table t(get_table_id(q.table_name), *this);
  t.select();  // todo
}

table_id DB::get_unique_id() {
  ++unique_id_counter;
  save_global_metadata();
  return unique_id_counter;
}

void DB::load_global_metadata() {
  std::fstream ifs(base_dir / "meta", std::ios::in | std::ios::binary);
  ifs.seekg(0, std::ios::end);
  std::streampos fsize = ifs.tellg();
  ifs.seekg(0, std::ios::beg);
  if (fsize == 0) {
    unique_id_counter = 0;
  } else {
    unique_id_counter = read_table_id(ifs);
  }
}
void DB::save_global_metadata() {
  std::fstream ifs(base_dir / "meta", std::ios::out | std::ios::binary);
  write_table_id(ifs, unique_id_counter);
}

table_id DB::get_table_id(std::string name) {
  if (name == "_last") {
    return unique_id_counter;
  }
  return atoi(name.c_str() + 1);
}

void DB::set_silent(bool val) {
  silent = val;
}