#include <cstring>
#include <fstream>

#include "serialize.h"
#include "DBTypes.h"

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
