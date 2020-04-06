#include "Index.h"

#include <optional>
#include <iostream>
#include <numeric>
#include <fstream>

#include "DBTypes.h"
#include "errors.h"
#include "serialize.h"

void print_tabs(int n) {
  for (int i = 0; i < n; i++) {
    std::cout << "  ";
  }
}

MemNode::MemNode(std::fstream& file, bool is_leaf, Node next): is_leaf{is_leaf}, next{next}, size{0} { // create
  file.seekp(0, std::ios::end);
  fnode = file.tellp();
  write_Node(file, *this);
}
MemNode::MemNode(std::fstream& file, Node fnode): fnode {fnode} { // load
  file.seekg(fnode, std::ios::beg);
  read_Node(file, *this);
}

void MemNode::insert(std::fstream& file, Pair pair) {
  assertSystem(size < capacity, "Trying to insert in full block");
  Key key = pair.key;
  int i = size - 1;
  for (; i >= 0; i--) {
    if (block[i].key > key) {
      block[i + 1] = block[i];
    } else {
      break;
    }
  }
  block[i + 1] = pair;
  size++;
}

void MemNode::sync_with_fs(std::fstream& file) {
  file.seekp(fnode, std::ios::beg);
  write_Node(file, *this);
}

void print_tree(std::fstream& file, Node fnode, int level) {
  MemNode node(file, fnode);
  if (node.is_leaf) {
    std::cout << "[";
    for (int i = 0; i < capacity; i++) {
      if (i < node.size) {
        std::cout << node.block[i].key << ":" << node.block[i].node;
      } else {
        std::cout << "_";
      }
      if (i != capacity - 1) {
        std::cout << "|";
      }
    }
    std::cout << "]";
    std::cout << "[>>" << node.next << "]";
  } else {
    std::cout << "(";
    for (int i = 0; i < capacity; i++) {
      if (i < node.size) {
        if (i != 0) {
          std::cout << node.block[i].key << "|";
        } else {
          std::cout << "";
        }
        std::cout << node.block[i].node << ":";
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
    for (int i = 0; i < node.size; i++) {
      print_tabs(level + 1);
      std::cout << node.block[i].node << ":";
      print_tree(file, node.block[i].node, level + 2);
    }
    print_tabs(level);
    std::cout << "}";
  }
  std::cout << std::endl;
}

INT64Index::INT64Index(fs::path path, bool create) {
  if (create) {
    file.open(path, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
    write_root_ref();
    root = MemNode(file, true, NODE_END).fnode;
    write_root_ref();
  } else {
    file.open(path, std::ios::in | std::ios::out | std::ios::binary);
    read_root_ref();
  }
}

INT64Index::INT64Index(INT64Index&& old) {
  file = std::move(old.file);
  root = old.root;
}

void INT64Index::write_root_ref() {
  file.seekp(0, std::ios::beg);
  write_any(file, &root, sizeof(root));
}

void INT64Index::read_root_ref() {
  file.seekg(0, std::ios::beg);
  read_any(file, &root, sizeof(root));
}

Node INT64Index::search_node(Key key, bool deep, Node fnode) {
  if (fnode == NODE_END) {
    fnode = root;
  }
  MemNode node(file, fnode);
  std::optional<Pair> to_insert;
  if (!node.is_leaf) { // intermediary node - search in children
    for (int i = 1; i < node.size; i++) { // search child
      Pair& knp = node.block[i];
      if (key < knp.key) {
        return search_node(key, deep, node.block[i - 1].node);
      } else if (i == node.size - 1) {
        return search_node(key, deep, node.block[i].node);
      }
    }
  }
  if(!deep) {
    return node.fnode;
  }
  // search for record
  for (int i = 0; i < node.size; i++) {
    if (node.block[i].key == key) {
      return node.block[i].node;
    }
  }
  return NODE_END; // not found
}

std::pair<Node, int> INT64Index::search_record(Key key, Node fnode) {
    if (fnode == NODE_END) {
    fnode = root;
  }
  MemNode node(file, fnode);
  std::optional<Pair> to_insert;
  if (!node.is_leaf) { // intermediary node - search in children
    for (int i = 1; i < node.size; i++) { // search child
      Pair& knp = node.block[i];
      if (key < knp.key) {
        return search_record(key, node.block[i - 1].node);
      } else if (i == node.size - 1) {
        return search_record(key, node.block[i].node);
      }
    }
  }
  Node found = node.fnode;
  // search for record
  for (int i = 0; i < node.size; i++) {
    if (node.block[i].key >= key) {
      return {found, i};
    }
  }
  throw SystemError("This should never happen");
}

std::optional<Pair> INT64Index::insert(Key key, int offset, Node fnode) {
  if (fnode == NODE_END) {
    fnode = root;
  }
  MemNode node(file, fnode);
  std::optional<Pair> to_insert;
  if (!node.is_leaf) { // intermediary node - insert in children
    for (int i = 1; i < node.size; i++) { // search child
      Pair& knp = node.block[i];
      if (key < knp.key) {
        to_insert = insert(key, offset, node.block[i - 1].node);
        break;
      } else if (i == node.size - 1) {
        to_insert = insert(key, offset, node.block[i].node);
      }
    }
  }
  // exiting recursion
  if (!to_insert.has_value() && !node.is_leaf) { // if nothing to insert from children and not a leaf (leafs always insert)
    return {};
  }
  Pair pair = to_insert.value_or(Pair{key, offset});
  int size = node.size;
  if (size < capacity) { // insert directly
    node.insert(file, pair);
    node.sync_with_fs(file);
    return {};
  }
  // otherwise need to split
  int split_point = size / 2;
  MemNode new_node = MemNode(file, node.is_leaf, node.next);
  node.next = new_node.fnode;
  for (int i = split_point; i < size; i++) { // move half to new node
    new_node.block[i - split_point] = node.block[i];
  }
  new_node.size = split_point;
  node.size -= split_point;
  // insert middle elem
  if(pair.key < new_node.block[0].key) {
    node.insert(file, pair);
  } else {
    new_node.insert(file, pair);
  }

  node.sync_with_fs(file);
  new_node.sync_with_fs(file);

  // if root needs to split, make new root
  if (node.fnode == root) {
    MemNode new_root(file, false, NODE_END);
    new_root.insert(file, Pair{new_node.block[0].key, new_node.fnode});
    new_root.insert(file, Pair{MIN_KEY, node.fnode});
    root = new_root.fnode;
    write_root_ref();
    new_root.sync_with_fs(file);
    return {};
  }

  // return key to be inserted to parent
  return Pair{new_node.block[0].key, new_node.fnode};
}

INT64Index::~INT64Index() {

}