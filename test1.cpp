#include <iostream>
#include <type_traits>
#include <variant>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <stdlib.h>
#include "lib/IndexFS.h"
#include "lib/errors.h"

int main() {
  auto t1 = std::chrono::high_resolution_clock::now();
  std::fstream f("/tmp/testfile", std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
  INT64Index index(f);

  int num_records = 50000000;
  int random_record_idx = rand() % num_records;
  int key, value;
  for (int i = 0; i < num_records; i++) {
    int rkey = rand();
    int rvalue = rand();
    index.insert(rkey, rvalue);
    if (i == random_record_idx) {
      key = rkey;
      value = rvalue;
    }
  }
  auto t2 = std::chrono::high_resolution_clock::now();

  assertSystem(index.search_node(key) == value, "Error!");

  auto t3 = std::chrono::high_resolution_clock::now();
  auto duration = (t2 - t1);
  auto per_record = (t2 - t1) / (float) num_records;
  std::cout << 
    "insertion: " << std::chrono::duration_cast<std::chrono::microseconds>( duration ).count() << "micro seconds (" <<
    << std::chrono::duration_cast<std::chrono::microseconds>( per_record ).count() << " per record)" <<
    "\nsearch: " << std::chrono::duration_cast<std::chrono::microseconds>( t3 - t2 ).count() << "micro seconds";
}