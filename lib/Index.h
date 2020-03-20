

template<class Key>
struct PKPair {
  int size;
  bool is_last;
  void* pointer;
  Key key;
};


template<unsigned capacity, class Key>
class Node {
  int size;
  PKPair<Key> block[capacity];
};

template<class Key>
struct LeafPair {
  int offset;
  Key key;
  LeafPair* next;
};

template<unsigned capacity, class Key>
class LeafNode {
  int size;
  LeafPair<Key> block[capacity];  
};

template<unsigned capacity, class Key>
class Index {
  Node<capacity, Key>* root;
  public:
  Index() {
    root = new Node<capacity, Key>;
  }

  LeafPair choose_leaf(Node node, Key key) {
    for (int i = 0; i < node.size; i++) {
      if (node.block[i] == key) {
        return no
      }
    }

  }

  LeafPair search(Key key, Node node = root) {
    if (node.is_last) {
      return choose_leaf(node, key);
    }
  }

  ~Index() {
    delete root;
  }
};