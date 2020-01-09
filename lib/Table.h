#pragma once

#include <filesystem>

namespace fs = std::filesystem;

#include "DB.h"
#include "DBTypes.h"

class Table
{
public:
  const table_id id;
  static Table create (const std::string& name, const std::vector <Field>& fields, DB& db);
  static Table load (const std::string& name, DB const & db);
  void insert (int value);
private:
  fs::path data_path;
  fs::path page_map_path;
  fs::path toast_path;
  fs::path meta_path;
  std::fstream meta_file;
  std::fstream data_file;
  std::fstream page_map_file;
  std::fstream toast_file;
  const DB& db;
  Table (const table_id id, const DB& db);
  void create (std::string const & name, std::vector <Field> const & fields);
  void load (std::string const & name);
};