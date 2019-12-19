#pragma once

#include <filesystem>
#include <fstream>
#include "DBTypes.hpp"
namespace fs = std::filesystem;

class Table {
 public:
  const table_id id;

  static Table create (const table_id id, const fs::path files_dir) {
    Table table = Table{id, files_dir};
    table.create();
    return table;
  }

  static Table load (const table_id id, const fs::path files_dir) {
    Table table = Table{id, files_dir};
    table.load();
    return table;
  }

  void insert(int value) {
    std::cout << "inserting " << value << std::endl;
    data_file.write(reinterpret_cast<char*>(&value), sizeof(value)); //todo real serialize
  }

 private:
  const fs::path data_path;
  const fs::path page_map_path;
  const fs::path toast_path;
  const fs::path meta_path;

  std::fstream meta_file;
  std::fstream data_file;
  std::fstream page_map_file;
  std::fstream toast_file;
  
  Table (const table_id id, const fs::path files_dir)
    : id { id },
      data_path     { files_dir / "data"     },
      page_map_path { files_dir / "page_map" },
      toast_path    { files_dir / "toast"    },
      meta_path     { files_dir / "meta"     } {
  }

  void create() {
    auto mode = std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary;
    meta_file.open(meta_path, mode);
    data_file.open(data_path, mode);
    page_map_file.open(page_map_path, mode);
    toast_file.open(toast_path, mode);
  }

  void load() {
    auto mode = std::ios::in | std::ios::out | std::ios::app | std::ios::binary;
    meta_file.open(meta_path, mode);
    data_file.open(data_path, mode);
    page_map_file.open(page_map_path, mode);
    toast_file.open(toast_path, mode);
  }
};