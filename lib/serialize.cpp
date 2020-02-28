#include <cstring>
#include <fstream>

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