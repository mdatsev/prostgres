#include <iostream>
#include <type_traits>
#include <variant>
#include <filesystem>
#include <fstream>
#include <stdlib.h>
#include "lib/Index.h"

int main() {
  INT64Index index;

  for (int i = 0; i < 100; i++) {
    int r = rand() % 100;
    index.insert(r, r);
  }
  index.root->print();
}