#include <iostream>
#include <type_traits>
#include <variant>
#include <filesystem>
#include <fstream>
#include <stdlib.h>
#include "lib/Index.h"

int main() {
  std::fstream f("testfile", std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
  INT64Index index(f, true);
  for (int i = 0; i < 100; i++) {
    int r = rand() % 100;
    index.insert(r, r);
  }
  print_tree(f, index.root);
}