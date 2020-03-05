#pragma once

#include <filesystem>

namespace fs = std::filesystem;

#include "DB.h"
#include "DBTypes.h"
#include "serialize.h"

class Table {
 public:
  const table_id id;
  static Table create(const std::string& name, const std::vector<Field>& fields, DB& db);
  static Table load(table_id name, DB const& db);
  void insert(std::vector<DBValue> row);
  void select(); // todo
  int32_t nfields;

 private:
  RowSerializer row_serializer;
  fs::path data_path;
  fs::path page_map_path;
  fs::path toast_path;
  fs::path meta_path;
  std::fstream meta_file;
  std::fstream data_file;
  std::fstream page_map_file;
  std::fstream toast_file;
  const DB& db;
  Table(const table_id id, const DB& db);
  void create(std::string const& name, std::vector<Field> const& fields);
  void load(table_id id);
};