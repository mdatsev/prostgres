#pragma once

#include <filesystem>
#include <unordered_map>

namespace fs = std::filesystem;

#include "DBTypes.h"
#include "CreateQuery.h"
#include "InsertQuery.h"
#include "SelectQuery.h"

class DB
{
public:
  fs::path const base_dir;
  fs::path const tables_dir;
  const std::unordered_map <std::string, table_id> table_ids;
  DB (fs::path base_dir);
  void execute (CreateQuery q);
  void execute (InsertQuery q);
  void execute (SelectQuery q);
  table_id get_unique_table_id ();
  field_id get_unique_field_id ();
private:
  int64_t unique_id_counter;
  void initialize_file_structure (fs::path const base_dir);
  void load_global_metadata (fs::path const base_dir);
  void save_global_metadata (fs::path const base_dir);
};
