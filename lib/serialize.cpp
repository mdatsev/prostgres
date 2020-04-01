#include <cstring>
#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>

#include "serialize.h"
#include "DBTypes.h"
#include "errors.h"
#include "Index.h"

void serialize_table_id(table_id *value, char(storage)[])
{
  std::memcpy((void *)storage, (void *)value, sizeof(value));
}
void deserialize_table_id(table_id *value, char(storage)[])
{
  std::memcpy((void *)value, (void *)storage, sizeof(value));
}
void write_table_id(std::fstream &file, table_id value)
{
  char storage[sizeof(value)];
  serialize_table_id(&value, storage);
  file.write(storage, sizeof(value));
}
table_id read_table_id(std::fstream &file)
{
  table_id value;
  char storage[sizeof(table_id)];
  file.read(storage, sizeof(table_id));
  deserialize_table_id(&value, storage);
  return value;
}

void serialize_DBValue (const DBValue& value, char (storage) []) {
  assertSystem(value.index() == (int)DBType::int64, "Unsupported type!"); // only supporting int64 for now
  auto& raw_value = std::get<(int)DBType::int64>(value);
  std::memcpy((void *)storage, (void *)&raw_value, sizeof(INT64_type)); // todo get the actual type
}

void write_DBValue (std::fstream & file, const DBValue& value) {
  char storage[sizeof(INT64_type)]; // for now todo
  serialize_DBValue(value, storage);
  file.write(storage, sizeof(INT64_type)); // for now todo
}

void serialize_meta_int(meta_int value, char(storage)[]) {
  std::memcpy((void *)storage, (void *)&value, sizeof(value));
}

void write_meta_int (std::fstream & file, meta_int value) {
  char storage[sizeof(value)];
  serialize_meta_int(value, storage);
  file.write(storage, sizeof(value));
}

void write_any(std::fstream& file, const void* data, int size) {
  file.write((char*)data, size);
}

void read_any(std::fstream& file, void* data, int size) {
  file.read((char*)data, size);
}

void deserialize_meta_int(meta_int& value, char(storage)[]) {
  std::memcpy((void *)&value, (void *)storage, sizeof(value));
}

meta_int read_meta_int (std::fstream & file) {
  meta_int value;
  char storage[sizeof(value)];
  file.read(storage, sizeof(value));
  deserialize_meta_int(value, storage);
  return value;
}

RowSerializer::RowSerializer(std::vector<DBType> types) : types {types} {
  // assertSystem(types.size() != 0, "Empty row!");
}

void RowSerializer::read_packed_row(std::fstream &file, char storage[]) {
  char* curr = storage;
  for (auto& t : types) {
    assertUser(t == DBType::int64, "Unsupported type in table");
    file.read(curr, sizeof(INT64_type));
    curr += sizeof(INT64_type);
  }
}
void RowSerializer::print_packed_row(char storage[]) {
  char* curr = storage;
  std::cout << "|";
  for (auto& t : types) {
    assertUser(t == DBType::int64, "Unsupported type in table");
    INT64_type value;
    std::memcpy((void*)&value, curr, sizeof(INT64_type));
    std::cout << std::setw(9) << std::to_string(value) << "|";
    curr += sizeof(INT64_type);
  }
}

std::string read_string(std::fstream& file) {
  meta_int size = read_meta_int(file);
  std::string s;
  s.reserve(size);
  file.read(&s[0], size);
  return s;
}
void write_string(std::fstream& file, const std::string& s) {
  meta_int size = s.size();
  write_meta_int(file, size);
  file.write(s.c_str(), size);
}

void RowSerializer::print_row(std::fstream &file, std::vector<int> ics) {
  std::cout << "|";
  for (int i = 0; i < types.size(); i++) {
    bool skip = ics.size() != 0 && std::find(ics.begin(), ics.end(), i) == ics.end();
    auto&& t = types[i];
    if(t == DBType::int64) {
      char buff[sizeof(INT64_type)];
      file.read(buff, sizeof(INT64_type));
      if(skip)continue;
      std::cout << std::setw(9) << std::to_string(*(INT64_type*)buff) << "|";
    } else if (t == DBType::string) {
      meta_int size = read_meta_int(file);
      char buff[size + 1];
      file.read(buff, size);
      buff[size] = '\0';
      if(skip)continue;
      std::cout << std::setw(9) << buff << "|";
    } else if (t == DBType::table_id) {
      char buff[sizeof(table_id)];
      file.read(buff, sizeof(table_id));
      if(skip)continue;
      std::cout << std::setw(9) << std::to_string(*(table_id*)buff) << "|";
    } else {
      throw UserError("Unsupported type in table");
    }
  }
}

void RowSerializer::write_row(std::fstream &file, std::vector<std::string> literals, std::vector<INT64Index>& indexes) {
  assertUser(literals.size() == types.size(), "Invalid number of values");
  int offset = file.tellp();
  for (int i = 0; i < types.size(); i++) {
    if(types[i] == DBType::int64) {
      INT64_type value = atoll(literals[i].c_str());
      file.write((char*)&value, sizeof(INT64_type));
      indexes[i].insert(value, offset);
    } else if (types[i] == DBType::string) {
      meta_int size = literals[i].size();
      write_meta_int(file, size);
      file.write(literals[i].c_str(), size);
    } else if (types[i] == DBType::table_id) {
      table_id value = atoll(literals[i].c_str());
      file.write((char*)&value, sizeof(table_id));
    } else {
      throw UserError("Unsupported type in table");
    }
  }
}

void RowSerializer::write_row(std::fstream &file, std::vector<DBValue> row, std::vector<INT64Index>& indexes) {
  assertUser(row.size() == types.size(), "Invalid number of values");
  int offset = file.tellp();
  for (int i = 0; i < types.size(); i++) {
    if(types[i] == DBType::int64) {
      INT64_type value = std::get<(int)DBType::int64>(row[i]);
      file.write((char*)&value, sizeof(INT64_type));
      indexes[i].insert(value, offset);
    } else if (types[i] == DBType::string) {
      auto str = std::get<(int)DBType::string>(row[i]);
      meta_int size = str.size();
      write_meta_int(file, size);
      file.write(str.c_str(), size);
    } else if (types[i] == DBType::table_id) {
      table_id value = std::get<(int)DBType::table_id>(row[i]);
      file.write((char*)&value, sizeof(table_id));
    } else {
      throw UserError("Unsupported type in table");
    }
  }
}

std::vector<DBValue> RowSerializer::read_row(std::fstream &file) {
  std::vector<DBValue> result;
  for (int i = 0; i < types.size(); i++) {
    if(types[i] == DBType::int64) {
      INT64_type value;
      file.read((char*)&value, sizeof(INT64_type));
      result.push_back(DBValue(std::in_place_index<(int)DBType::int64>, value));
    } else if (types[i] == DBType::string) {
      meta_int size = read_meta_int(file);
      std::string s;
      s.reserve(size);
      file.read(&s[0], size);
      result.push_back(s);
    } else if (types[i] == DBType::table_id) {
      table_id value;
      file.read((char*)&value, sizeof(meta_int));
      result.push_back(DBValue(std::in_place_index<(int)DBType::table_id>, value));
    } else {
      throw UserError("Unsupported type in table");
    }
  }
  return result;
}
void write_Node(std::fstream& file, const MemNode& node) {
  file.write((char*)&node, sizeof(MemNode));
}
void read_Node(std::fstream& file, MemNode& node) {
  file.read((char*)&node, sizeof(MemNode));
}