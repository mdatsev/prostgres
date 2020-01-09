#include <filesystem>
#include <fstream>

#include "Table.h"
#include "DB.h"
#include "DBTypes.h"

/* static */ Table Table::create(const std::string &name, const std::vector<Field> &fields, DB &db) {
  Table table = Table{db.get_unique_id(), db};
  table.create(name, fields);
  return table;
}
/* static */ Table Table::load(std::string const &name, DB const &db) {
  Table table = Table{1, db};
  // table.load(name);
  return table;
}

void Table::insert(int value) {}
Table::Table(table_id const id, DB const &db) : id{id}, db{db} {
  auto table_dir = db.tables_dir / std::to_string(id);
  fs::create_directory(table_dir);

  auto mode = std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary;

  meta_path = table_dir / "meta";
  meta_file.open(meta_path, mode);

  data_path = table_dir / "data";
  data_file.open(data_path, mode);

  page_map_path = table_dir / "page_map";
  page_map_file.open(page_map_path, mode);

  toast_path = table_dir / "toast";
  toast_file.open(toast_path, mode);
}

void Table::create(std::string const &name, std::vector<Field> const &fields) {
  auto mode = std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary;
  meta_file.open(meta_path, mode);
  data_file.open(data_path, mode);
  page_map_file.open(page_map_path, mode);
  toast_file.open(toast_path, mode);
}
void Table::load(std::string const &name) {
  auto mode = std::ios::in | std::ios::out | std::ios::app | std::ios::binary;
  meta_file.open(meta_path, mode);
  data_file.open(data_path, mode);
  page_map_file.open(page_map_path, mode);
  toast_file.open(toast_path, mode);
}