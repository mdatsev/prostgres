#include <iostream>
#include <fstream>
#include <unordered_map>

#include "DB.h"
#include "serialize.h"

DB::DB (fs::path base_dir) 
    : base_dir { base_dir },
      tables_dir { base_dir / "tables" } {
    initialize_file_structure(base_dir);
    load_global_metadata(base_dir);
  }
void DB::execute (CreateQuery q)
                               {
    std::cout << "created table " << q.table_name << "(";
    for(auto field : q.fields) {
      std::cout << field.name << " " << toString(field.type) << ", ";
    }
    std::cout << ");\n" << std::endl;
    // Table::create(q.table_name, q.fields, this);
  }
void DB::execute (InsertQuery q)
                               {
    std::cout << "inserted into table " << q.table_name << "(";
    for(auto field : q.fields) {
      std::cout << field << ", ";
    }
    std::cout << ") values (";
    for(auto value : q.values) {
      std::cout << value << ", ";
    }
    std::cout << ");\n" << std::endl;

    // auto t = Table::load(1, tables_dir / "1");
    // for(auto value : q.values) {
    //   t.insert(atoi(value.data()));
    // }
  }
void DB::execute (SelectQuery q)
                               {
    std::cout << "selected ";
    for(auto field : q.fields) {
      std::cout << field << ", ";
    }
    std::cout << "from " << q.table_name << ";\n";
  }
table_id DB::get_unique_table_id ()
                                  {
    return ++unique_id_counter;
  }
field_id DB::get_unique_field_id ()
                                  {
    return ++unique_id_counter;
  }
void DB::initialize_file_structure (fs::path const base_dir)
                                                           {
    fs::create_directory(base_dir);
    fs::create_directory(tables_dir);
  }
void DB::load_global_metadata (fs::path const base_dir)
                                                     {
    std::fstream ifs(base_dir / "meta", std::ios::in);
    if (ifs.tellg() == 0) {
      unique_id_counter = 0;
    } else {
      unique_id_counter = read_table_id(ifs);
    }
  }
void DB::save_global_metadata (fs::path const base_dir)
                                                     {
    std::fstream ifs(base_dir / "meta", std::ios::in);
    write_table_id(ifs, unique_id_counter);
  }
#undef LZZ_INLINE
