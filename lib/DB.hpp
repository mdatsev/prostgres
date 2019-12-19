#pragma once
#include <iostream>
#include <fstream>
#include "Table.hpp"
#include "CreateQuery.hpp"
#include "InsertQuery.hpp"
#include "SelectQuery.hpp"

class DB {
 public:
  const fs::path base_dir;
  const fs::path tables_dir;
  
  DB (fs::path base_dir)
    : base_dir { base_dir },
      tables_dir { base_dir / "tables" } {
    initialize_file_structure(base_dir);
    load_global_metadata(base_dir);
  }

  void execute (CreateQuery q) {
    std::cout << "created table " << q.table_name << "(";
    for(auto field : q.fields) {
      std::cout << field.name << " " << toString(field.type) << ", ";
    }
    std::cout << ");\n" << std::endl;
  }

  void execute (InsertQuery q) {
    // std::cout << "inserted into table " << q.table_name << "(";
    // for(auto field : q.fields) {
    //   std::cout << field << ", ";
    // }
    // std::cout << ") values (";
    // for(auto value : q.values) {
    //   std::cout << value << ", ";
    // }
    // std::cout << ");\n" << std::endl;

    auto t = Table::load(1, tables_dir / "1");
    for(auto value : q.values) {
      t.insert(atoi(value.data()));
    }
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

  void load_global_metadata(const fs::path base_dir) {
    std::ifstream ifs(base_dir / "id_counter.txt", std::ios::in);

    ifs >> unique_id_counter;
  }
};