#include <filesystem>
#include <fstream>

#include "DB.h"
#include "DBTypes.h"
#include "Table.h"
#include "serialize.h"

/* static */ Table Table::create(const std::string &name, const std::vector<Field> &fields, DB &db) {
  Table table = Table{db.get_unique_id(), db};
  return table;
}
/* static */ Table Table::load(table_id id, DB const &db) {
  Table table = Table{id, db};
  table.load(id);
  return table;
}

void Table::insert(std::vector<DBValue> row) {
  data_file.seekg(0, std::ios::end);
  for (auto &value : row) {
    write_DBValue(data_file, value);
  }
}

Table::Table(table_id const id, DB const &db) : id{id}, db{db} {
  auto table_dir = db.tables_dir / std::to_string(id);
  fs::create_directory(table_dir);

  auto mode = std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary;

  meta_path = table_dir / "meta";

  data_path = table_dir / "data";

  page_map_path = table_dir / "page_map";

  toast_path = table_dir / "toast";
}

void Table::create(std::string const &name, std::vector<Field> const &fields) {
  auto mode = std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary;
  meta_file.open(meta_path, mode);
  data_file.open(data_path, mode);
  page_map_file.open(page_map_path, mode);
  toast_file.open(toast_path, mode);

  write_meta_int(meta_file, fields.size());
}

void Table::load(table_id id) {
  auto mode = std::ios::in | std::ios::out | std::ios::binary;
  meta_file.open(meta_path, mode);
  data_file.open(data_path, mode);
  page_map_file.open(page_map_path, mode);
  toast_file.open(toast_path, mode);
  nfields = read_meta_int(meta_file);
}
