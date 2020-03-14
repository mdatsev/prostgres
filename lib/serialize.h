#pragma once

#include "DBTypes.h"

void serialize_DBValue(DBValue* value, char(storage)[]);

void serialize_table_id(table_id* value, char(storage)[]);
void deserialize_table_id(table_id* value, char(storage)[]);

void write_table_id(std::fstream& file, table_id value);
table_id read_table_id(std::fstream& file);

void write_DBValue(std::fstream& file, const DBValue& value);

void serialize_meta_int(meta_int value, char(storage)[]);
void write_meta_int(std::fstream& file, meta_int value);
void deserialize_meta_int(meta_int& value, char(storage)[]);
meta_int read_meta_int(std::fstream& file);

class RowSerializer {
  std::vector<DBType> types;
 public:
  RowSerializer(std::vector<DBType> types = {});
  void read_packed_row(std::fstream &file, char storage[]);
  void print_packed_row(char storage[]);
  void print_row(std::fstream &file);
  void write_row(std::fstream &file, std::vector<std::string> literals);
  void write_row(std::fstream &file, std::vector<DBValue> row);
};