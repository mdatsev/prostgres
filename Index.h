#include <optional>
#include <numeric>

#include "DBTypes.h"

using Key = INT64_type;
constexpr int capacity = 4000;
constexpr Key MIN_KEY = std::numeric_limits<Key>::min();

class Node;

struct KeyNodePair {
  Key key;
  Node* pointer;
};

struct KeyRecordPair {
  Key key;
  int offset;
};

union Pair {
  KeyNodePair keynode;
  KeyRecordPair keyrecord;
};

Key get_key(Pair pair, bool is_leaf);

void print_tabs(int n);
struct Node {
  int size = 0;
  bool is_leaf = false;
  Pair block[capacity];
  Node* next;
  void insert(Pair pair);
  Key get_key(int index);
  void print(int level = 0);
};


class INT64Index {
 public:
  Node* root;
 public:
  INT64Index();

  Node* search_node(Key key, Node* node);

  std::optional<Pair> insert(Key key, int offset, Node* node = nullptr);
  
  ~INT64Index();
};