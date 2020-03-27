#include "Index.h"

#include <optional>
#include <iostream>
#include <numeric>

#include "DBTypes.h"
#include "errors.h"

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

void print_tabs(int n) {
  for (int i = 0; i < n; i++) {
    std::cout << "  ";
  }
}
void Node::insert(Pair pair) {
  assertSystem(size < capacity, "Trying to insert in full block");
  Key key = ::get_key(pair, is_leaf);
  int i = size - 1;
  for (; i >= 0; i--) {
    if (get_key(i) > key) {
      block[i + 1] = block[i];
    } else {
      break;
    }
  }
  block[i + 1] = pair;
  size++;
}
Key Node::get_key(int index) {
  return is_leaf ? block[index].keyrecord.key : block[index].keynode.key;
}
void Node::print(int level = 0) {
  if (is_leaf) {
    std::cout << "[";
    for (int i = 0; i < capacity; i++) {
      if (i < size) {
        std::cout << get_key(i) << ":" << block[i].keyrecord.offset;
      } else {
        std::cout << "_";
      }
      if (i != capacity - 1) {
        std::cout << "|";
      }
    }
    std::cout << "]";
  } else {
    std::cout << "(";
    for (int i = 0; i < capacity; i++) {
      if (i < size) {
        if (i != 0) {
          std::cout << get_key(i) << "|";
        } else {
          std::cout << "";
        }
        std::cout << i << ":";
      } else {
        std::cout << "_|_";
      }
      if (i != capacity - 1) {
        std::cout << "|";
      }
    }
    std::cout << ")\n";
    print_tabs(level);
    std::cout << "{\n";
    for (int i = 0; i < size; i++) {
      print_tabs(level + 1);
      std::cout << i << ":";
      block[i].keynode.pointer->print(level + 2);
    }
    print_tabs(level);
    std::cout << "}";
  }
  std::cout << std::endl;
}

INT64Index::INT64Index() {
  root = new Node;
  root->is_leaf = true;
}

Node* INT64Index::search_node(Key key, Node* node = nullptr) {
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

std::optional<Pair> INT64Index::insert(Key key, int offset, Node* node = nullptr) {
  if (node == nullptr) {
    node = root;
  }
  std::optional<Pair> to_insert;
  if (!node->is_leaf) {
    for (int i = 1; i < node->size; i++) {
      KeyNodePair& knp = node->block[i].keynode;
      if (key < knp.key) {
        to_insert = insert(key, offset, node->block[i - 1].keynode.pointer);
        break;
      } else if (i == node->size - 1) {
        to_insert = insert(key, offset, node->block[i].keynode.pointer);
      }
    }
  }
  if (!to_insert.has_value() && !node->is_leaf) {
    return {};
  }
  Pair pair = to_insert.value_or(Pair{.keyrecord={key, offset}});
  int size = node->size;
  if (size < capacity) {
    node->insert(pair);
  } else {
    int split_point = size / 2;
    Node* new_node = new Node;
    for (int i = split_point; i < size; i++) {
      new_node->block[i - split_point] = node->block[i];
    }
    new_node->size = split_point;
    node->size -= split_point;
    new_node->is_leaf = node->is_leaf;
    if(get_key(pair, node->is_leaf) < new_node->get_key(0)) {
      node->insert(pair);
    } else {
      new_node->insert(pair);
    }
    if (node == root) {
      root = new Node;
      root->insert(Pair{.keynode={new_node->get_key(0), new_node}});
      root->insert(Pair{.keynode={MIN_KEY, node}});
    } else {
      return Pair{.keynode={new_node->get_key(0), new_node}};
    }
  }
  return {};
}

INT64Index::~INT64Index() {
  delete root;
}