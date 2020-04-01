#pragma once
#include <vector>

#include "DBTypes.h"
#include "Index.h"
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

void write_any(std::fstream& file, const void* data, int size);
void read_any(std::fstream& file, void* data, int size);
class RowSerializer {
  std::vector<DBType> types;
 public:
  RowSerializer(std::vector<DBType> types = {});
  void read_packed_row(std::fstream &file, char storage[]);
  void print_packed_row(char storage[]);
  void print_row(std::fstream &file, std::vector<int> = {});
  void write_row(std::fstream &file, std::vector<std::string> literals, std::vector<INT64Index>& indexes);
  void write_row(std::fstream &file, std::vector<DBValue> row, std::vector<INT64Index>& indexes);
  std::vector<DBValue> read_row(std::fstream &file);
};
std::string read_string(std::fstream& file);
void write_string(std::fstream& file, const std::string& s);
void write_Node(std::fstream& file, const MemNode& node);
void read_Node(std::fstream& file, MemNode& node);