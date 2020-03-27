#pragma once
#include <optional>
#include <numeric>

#include "DBTypes.h"

using Key = INT64_type;
using Node = meta_int;
constexpr int capacity = 4;
constexpr Key MIN_KEY = std::numeric_limits<Key>::min();

struct Pair {
  Key key;
  Node node;
};

void print_tabs(int n);
struct MemNode {
  MemNode(std::fstream& file, bool is_leaf); // create
  MemNode(std::fstream& file, Node fnode); // load
  void sync_with_fs(std::fstream& file); // load
  Node fnode;
  meta_int size = 0;
  bool is_leaf;
  Pair block[capacity];
  Node next;
  void insert(std::fstream& file, Pair pair);
  void print(int level = 0);
};


class INT64Index {
 public:
  Node root;
  std::fstream& file;
 public:
  INT64Index(std::fstream &file);

  Node search_node(Key key, Node node = -1);

  std::optional<Pair> insert(Key key, int offset, Node node = -1);
  
  ~INT64Index();
};

void print_tree(std::fstream& file, Node fnode, int level = 0);