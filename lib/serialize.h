#pragma once

#include "DBTypes.h"

void serialize_table_id (table_id * value, char (storage) []);
void deserialize_table_id (table_id * value, char (storage) []);
void write_table_id (std::fstream & file, table_id value);
table_id read_table_id (std::fstream & file);
