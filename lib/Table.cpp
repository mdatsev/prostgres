#include <iostream>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <sys/stat.h>
#include "DB.h"
#include "DBTypes.h"
#include "Table.h"
#include "serialize.h"

long GetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

/* static */ Table Table::create(const std::string &name, const std::vector<Field> &fields, DB &db) {
  Table table = Table{db.get_unique_id(), db};
  table.create(name, fields);
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
  write_meta_int(meta_file, fields.size());
  std::vector<DBType> types;
  std::transform(begin(fields), end(fields), std::back_inserter(types), [](auto&& f){return f.type;});
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
    types.push_back(INT64); // for now, todo get real types
  }
  row_serializer = RowSerializer(types);
}

void Table::select(){
  int curr = data_file.tellg();
  data_file.seekg (0, data_file.end);
  int length = data_file.tellg();
  data_file.seekg (curr, data_file.beg);
  char arr[row_serializer.storage_size()];
  for (int i = 0; i < length / sizeof(arr); i++) {
    row_serializer.read_packed_row(data_file, arr);
    row_serializer.print_packed_row(arr);
    std::cout << std::endl;
  }
}