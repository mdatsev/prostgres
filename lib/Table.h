#pragma once

#include <filesystem>

namespace fs = std::filesystem;

#include "DBTypes.h"
#include "serialize.h"
class DB;
class Table {
 public:
  const table_id id;
  Table(table_id id, const std::string& name, const std::vector<Field>& fields, DB& db); // create
  Table(table_id id, DB const& db); // load
  void insert(InsertQuery q);
  void insert(std::vector<DBValue> row);
  void select(SelectQuery q);
  std::vector<std::vector<DBValue>> select();
  int32_t nfields;

 private:
  std::vector<INT64Index> indexes;
  std::unordered_map<std::string, int> column_names;
  void load_paths();
  RowSerializer row_serializer;
  fs::path data_path;
  fs::path page_map_path;
  fs::path toast_path;
  fs::path meta_path;
  fs::path table_dir;
  fs::path indexes_dir;
  std::fstream meta_file;
  std::fstream data_file;
  std::fstream page_map_file;
  std::fstream toast_file;
  const DB& db;
  void create(std::string const& name, std::vector<Field> const& fields);
  void load(table_id id);
};