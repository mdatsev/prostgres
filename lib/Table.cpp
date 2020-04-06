#include <sys/stat.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "DB.h"
#include "DBTypes.h"
#include "Table.h"
#include "serialize.h"
long GetFileSize(std::string filename) {
  struct stat stat_buf;
  int rc = stat(filename.c_str(), &stat_buf);
  return rc == 0 ? stat_buf.st_size : -1;
}

void Table::insert(InsertQuery q) {
  data_file.seekg(0, std::ios::end);
  row_serializer.write_row(data_file, q.values, indexes);
}

void Table::insert(std::vector<DBValue> row) {
  data_file.seekg(0, std::ios::end);
  row_serializer.write_row(data_file, row, indexes);
}

Table::Table(table_id id, const std::string& name, const std::vector<Field>& fields, DB& db) : id{id}, db{db}  {
  load_paths();
  create(name, fields);
}

Table::Table(table_id id, DB const& db) : id{id}, db{db} {
  load_paths();
  load(id);
}

void Table::load_paths() {
  table_dir = db.tables_dir / std::to_string(id);
  fs::create_directory(table_dir);

  meta_path = table_dir / "meta";

  data_path = table_dir / "data";

  page_map_path = table_dir / "page_map";

  toast_path = table_dir / "toast";
  indexes_dir = table_dir / "indexes";
}

void Table::create(std::string const &name, std::vector<Field> const &fields) {
  auto mode = std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary;
  meta_file.open(meta_path, mode);
  data_file.open(data_path, mode);
  page_map_file.open(page_map_path, mode);
  toast_file.open(toast_path, mode);

  nfields = fields.size();
  write_meta_int(meta_file, nfields);
  meta_file.flush();
  std::vector<DBType> types;
  std::transform(begin(fields), end(fields), std::back_inserter(types), [](auto &&f) { return f.type; });
  for (auto t : types) {
    write_meta_int(meta_file, (int) t);
  }
  row_serializer = RowSerializer(types);
  fs::create_directory(indexes_dir);
  for (int i = 0; i < fields.size(); i++) {
    write_string(meta_file, fields[i].name);
    column_names[fields[i].name] = i;
    indexes.emplace_back(indexes_dir / std::to_string(i), true);
  }
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
    types.push_back((DBType)read_meta_int(meta_file));
  }
  row_serializer = RowSerializer(types);
  for (int i = 0; i < nfields; i++) {
    column_names[read_string(meta_file)] = i;
    indexes.emplace_back(indexes_dir / std::to_string(i), false);
  }
}

void Table::select(SelectQuery q) {
  std::vector<int> col_indices;
  std::transform(begin(q.fields), end(q.fields), std::back_inserter(col_indices), [&](auto &&f) { return column_names[f]; });
  if (q.conditional) {
    auto op = q.condition.op;
    int col_pos = column_names[q.table_name];
    auto&& ind = indexes[col_pos];
    auto key = atoll(q.condition.value.c_str());
    bool op_before = op == Op::ne || op == Op::le || op == Op::lt;
    bool op_eq = op == Op::eq || op == Op::le || op == Op::ge;
    bool op_after = op == Op::ne || op == Op::ge || op == Op::gt;
    bool started_seq = false;
    if (op_before) {
      auto fnode = ind.search_node(MIN_KEY);
      while(fnode != NODE_END) {
        MemNode node(ind.file, fnode);
        for (int i = 0; i < node.size; i++) {
          data_file.seekg(node.block[i].node, data_file.beg);
          auto row = row_serializer.read_row(data_file);
          bool cond_met = apply_op(std::get<(int)DBType::int64>(row[col_pos]), Op::lt, key);
          if (!cond_met) {
            break;
          }
          data_file.seekg(node.block[i].node, data_file.beg);
          row_serializer.print_row(data_file, col_indices);
          std::cout << '\n';
        }
        fnode = node.next;
      }
    }
    if (op_eq) {
      auto [fnode, first_pos] = ind.search_record(key);
      while(fnode != NODE_END) {
        MemNode node(ind.file, fnode);
        for (int i = first_pos; i < node.size; i++) {
          data_file.seekg(node.block[i].node, data_file.beg);
          auto row = row_serializer.read_row(data_file);
          bool cond_met = apply_op(std::get<(int)DBType::int64>(row[col_pos]), Op::eq, key);
          if (!cond_met) {
            break;
          }
          data_file.seekg(node.block[i].node, data_file.beg);
          row_serializer.print_row(data_file, col_indices);
          std::cout << '\n';
        }
        fnode = node.next;
      }
    }
    if (op_after) {
      auto [fnode, first_pos] = ind.search_record(key + 1);
      while(fnode != NODE_END) {
        MemNode node(ind.file, fnode);
        for (int i = first_pos; i < node.size; i++) {
          data_file.seekg(node.block[i].node, data_file.beg);
          auto row = row_serializer.read_row(data_file);
          bool cond_met = apply_op(std::get<(int)DBType::int64>(row[col_pos]), Op::gt, key);
          if (!cond_met) {
            break;
          }
          data_file.seekg(node.block[i].node, data_file.beg);
          row_serializer.print_row(data_file, col_indices);
          std::cout << '\n';
        }
        fnode = node.next;
      }
    }
  } else {
    // int curr = data_file.tellg();
    data_file.seekg(0, data_file.end);
    int length = data_file.tellg();
    data_file.seekg(0, data_file.beg);
    // char arr[row_serializer.storage_size()];
    for (int i = 0; data_file.tellg() < length; i++) {
      row_serializer.print_row(data_file, col_indices);
      std::cout << '\n';
    }
  }
}

std::vector<std::vector<DBValue>> Table::select() {
  std::vector<std::vector<DBValue>> result;
  // int curr = data_file.tellg();
  data_file.seekg(0, data_file.end);
  int length = data_file.tellg();
  data_file.seekg(0, data_file.beg);
  // char arr[row_serializer.storage_size()];
  for (int i = 0; data_file.tellg() < length; i++) {
    result.push_back(row_serializer.read_row(data_file));
    std::cout << '\n';
  }
  return result;
}