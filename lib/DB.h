#pragma once

#include <filesystem>
#include <unordered_map>
#include <memory>

namespace fs = std::filesystem;

#include "CreateQuery.h"
#include "DBTypes.h"
#include "InsertQuery.h"
#include "SelectQuery.h"
#include "Table.h"

class DB {
 public:
  fs::path const base_dir;
  fs::path const tables_dir;
  std::unordered_map<table_id, Table> loaded_tables;
  DB(fs::path base_dir);
  void execute(CreateQuery q);
  void execute(InsertQuery q);
  void execute(SelectQuery q);
  table_id get_unique_id();
  void set_silent(bool);
  ~DB();

 private:
  Table* names_table;
  std::unordered_map<std::string, table_id> table_names_map;
  Table& load_table(std::string name);
  bool silent;
  int64_t unique_id_counter;
  void initialize_file_structure();
  void load_global_metadata();
  void save_global_metadata();
  table_id get_table_id(std::string name);
};
