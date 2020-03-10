#include <cstring>
#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>

#include "serialize.h"
#include "DBTypes.h"
#include "errors.h"

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
  assertSystem(value.index() == INT64, "Unsupported type!"); // only supporting int64 for now
  auto& raw_value = std::get<INT64>(value);
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


size_t RowSerializer::storage_size() {
  return types.size() * sizeof(INT64_type); // for now todo
}

RowSerializer::RowSerializer(std::vector<DBType> types) : types {types} {
  // assertSystem(types.size() != 0, "Empty row!");
}

void RowSerializer::read_packed_row(std::fstream &file, char storage[]) {
  char* curr = storage;
  for (auto& t : types) {
    assertUser(t == INT64, "Unsupported type in table");
    file.read(curr, sizeof(INT64_type));
    curr += sizeof(INT64_type);
  }
}
void RowSerializer::print_packed_row(char storage[]) {
  char* curr = storage;
  std::cout << "|";
  for (auto& t : types) {
    assertUser(t == INT64, "Unsupported type in table");
    INT64_type value;
    std::memcpy((void*)&value, curr, sizeof(INT64_type));
    std::cout << std::setw(9) << std::to_string(value) << "|";
    curr += sizeof(INT64_type);
  }
}

void RowSerializer::print_row(std::fstream &file) {
  std::cout << "|";
  for (auto& t : types) {
    assertUser(t == INT64, "Unsupported type in table");
    char buff[sizeof(INT64_type)];
    file.read(buff, sizeof(INT64_type));
    std::cout << std::setw(9) << std::to_string(*(INT64_type*)buff) << "|";
  }
}

void RowSerializer::write_row(std::fstream &file, std::vector<std::string> literals) {
  for (auto& t : literals) {
    INT64_type value = atoll(t.c_str());
    file.write((char*)&value, sizeof(INT64_type));
  }
}