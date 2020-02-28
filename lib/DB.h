#pragma once

#include <filesystem>
#include <unordered_map>

namespace fs = std::filesystem;

#include "CreateQuery.h"
#include "DBTypes.h"
#include "InsertQuery.h"
#include "SelectQuery.h"

class DB {
 public:
  fs::path const base_dir;
  fs::path const tables_dir;
  const std::unordered_map<std::string, table_id> table_ids;
  DB(fs::path base_dir);
  void execute(CreateQuery q);
  void execute(InsertQuery q);
  void execute(SelectQuery q);
  table_id get_unique_id();

 private:
  int64_t unique_id_counter;
  void initialize_file_structure();
  void load_global_metadata();
  void save_global_metadata();
  table_id get_table_id(std::string name);
};
