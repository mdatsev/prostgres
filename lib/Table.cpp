#include <filesystem>
#include <fstream>

#include "Table.h"
#include "DB.h"
#include "DBTypes.h"

Table Table::create(const std::string& name, const std::vector<Field>& fields, const DB& db)
{
  // db.
  Table table = Table{1, db};
  // table.create(name, fields);
  return table;
}
Table Table::load(std::string const &name, DB const &db)
{
  Table table = Table{1, db};
  // table.load(name);
  return table;
}
void Table::insert(int value)
{
}
Table::Table(table_id const id, DB const & db) : id{id}, db{db}
{
}
void Table::create(std::string const &name, std::vector<Field> const &fields)
{
  auto mode = std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary;
  meta_file.open(meta_path, mode);
  data_file.open(data_path, mode);
  page_map_file.open(page_map_path, mode);
  toast_file.open(toast_path, mode);
}
void Table::load(std::string const &name)
{
  auto mode = std::ios::in | std::ios::out | std::ios::app | std::ios::binary;
  meta_file.open(meta_path, mode);
  data_file.open(data_path, mode);
  page_map_file.open(page_map_path, mode);
  toast_file.open(toast_path, mode);
}