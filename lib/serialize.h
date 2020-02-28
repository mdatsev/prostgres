#pragma once

#include "DBTypes.h"

void serialize_DBValue (DBValue * value, char (storage) []);

void serialize_table_id (table_id * value, char (storage) []);
void deserialize_table_id (table_id * value, char (storage) []);

void write_table_id (std::fstream & file, table_id value);
table_id read_table_id (std::fstream & file);

void write_DBValue (std::fstream & file, const DBValue& value);

void serialize_meta_int(meta_int value, char(storage)[]);
void write_meta_int (std::fstream & file, meta_int value);
void deserialize_meta_int(meta_int& value, char(storage)[]);
meta_int read_meta_int (std::fstream & file);