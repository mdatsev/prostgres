#include <sys/stat.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "DB.h"
#include "DBTypes.h"
#include "Table.h"
#include "serialize.h"
long GetFileSize(std::string filename) {
  struct stat stat_buf;
  int rc = stat(filename.c_str(), &stat_buf);
  return rc == 0 ? stat_buf.st_size : -1;
}

void Table::insert(InsertQuery q) {
  data_file.seekg(0, std::ios::end);
  row_serializer.write_row(data_file, q.values);
}

void Table::insert(std::vector<DBValue> row) {
  data_file.seekg(0, std::ios::end);
  row_serializer.write_row(data_file, row);
}

Table::Table(table_id id, const std::string& name, const std::vector<Field>& fields, DB& db) : id{id}, db{db}  {
  load_paths();
  create(name, fields);
}

Table::Table(table_id id, DB const& db) : id{id}, db{db} {
  load_paths();
  load(id);
}

void Table::load_paths() {
  auto table_dir = db.tables_dir / std::to_string(id);
  fs::create_directory(table_dir);

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

  nfields = fields.size();
  write_meta_int(meta_file, nfields);
  meta_file.flush();
  std::vector<DBType> types;
  std::transform(begin(fields), end(fields), std::back_inserter(types), [](auto &&f) { return f.type; });
  for (auto t : types) {
    write_meta_int(meta_file, (int) t);
  }
  row_serializer = RowSerializer(types);
}

void Table::load(table_id id) {
  auto mode = std::ios::in | std::ios::out | std::ios::binary;
  meta_file.open(meta_path, mode);
  data_file.open(data_path, mode);
  page_map_file.open(page_map_path, mode);
  toast_file.open(toast_path, mode);
  nfields = read_meta_int(meta_file);
  std::vector<DBType> types;
  for (int i = 0; i < nfields; i++) {
    types.push_back((DBType)read_meta_int(meta_file));
  }
  row_serializer = RowSerializer(types);
}

void Table::select(SelectQuery q) {
  // int curr = data_file.tellg();
  data_file.seekg(0, data_file.end);
  int length = data_file.tellg();
  data_file.seekg(0, data_file.beg);
  // char arr[row_serializer.storage_size()];
  for (int i = 0; data_file.tellg() < length; i++) {
    row_serializer.print_row(data_file);
    std::cout << '\n';
  }
}