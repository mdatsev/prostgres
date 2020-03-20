#include "DBTypes.h"

constexpr int capacity = 10;
using Key = INT64_type;

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

Key get_key(Pair pair, bool is_leaf) {
  return is_leaf ? pair.keyrecord.key : pair.keynode.key;
}

struct Node {
  int size;
  bool is_leaf;
  Pair block[capacity];
  Node* next;
  void insert(Pair pair) {
    assertSystem(size < capacity, "Trying to insert in full block");
    Key key = ::get_key(pair, is_leaf);
    for (int i = size - 1; i >= 0; i++) {
      if (get_key(i)) {
        block[i + 1] = block[i];
      } else {
        block[i + 1] = pair;
        break;
      }
    }
    size++;
  }
  Key get_key(int index) {
    return is_leaf ? block[index].keyrecord.key : block[index].keynode.key;
  }
};

class INT64Index {
  Node* root;
 public:
  INT64Index() {
    root = new Node;
  }

  Node* search_node(Key key, Node* node = nullptr) {
    if (node = NULL) {
      node = root;
    }
    if (node->is_leaf) { // leaf node, search record
      return node;
    }
    // intermediary node
    for (int i = 0; i < node->size - 1; i++) {
      KeyNodePair& knp = node->block[i].keynode;
      if (key < knp.key) {
        return search_node(key, knp.pointer);
      }
    }
    return search_node(key, node->block[node->size - 1].keynode.pointer);
  }

  void insert(Key key, int offset) {
    Node* node = search_node(key);
    insert({.keyrecord = {key, offset}}, node);
  }
  
  void insert(Pair pair, Node* node) {
    int size = node->size;
    if (size < capacity) {
      node->block[size] = pair;
    } else {
      int split_point = size / 2;
      Node* new_node = new Node;
      for (int i = split_point; i < size; i++) {
        new_node->block[i - split_point] = node->block[i];
      }
      new_node->size = split_point;
      new_node->is_leaf = node->is_leaf;
      if(get_key(pair, node->is_leaf) < new_node->get_key(0)) {
        node->insert(pair);
      } else {
        new_node->insert(pair);
      }
    }
  }

  ~INT64Index() {
    delete root;
  }
};

//       for (int i = 0; i < node->size; i++) {
//         KeyRecordPair& krp = node->block[i].keyrecord;
//         if (krp.key == key) {
//           return &krp;
//         }
//       }
//       return nullptr; // no record found
