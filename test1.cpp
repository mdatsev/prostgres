#include <iostream>
#include <type_traits>
#include <variant>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <stdlib.h>
#include "lib/Index.h"
#include "lib/errors.h"

int main() {
  int num_records = 1000000;
  int key, value;
  auto t1 = std::chrono::high_resolution_clock::now();
  {
    std::fstream f("/tmp/testfile", std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
    INT64Index index(f, true);

    int random_record_idx = rand() % num_records;
    for (int i = 0; i < num_records; i++) {
      int rkey = rand();
      int rvalue = rand();
      index.insert(rkey, rvalue);
      if (i == random_record_idx) {
        key = rkey;
        value = rvalue;
      }
      if (i % 100000 == 0) {
        std::cout << i << "\n";
      } 
    }
  }
  auto t2 = std::chrono::high_resolution_clock::now();
  {
    std::fstream f("/tmp/testfile", std::ios::in | std::ios::out | std::ios::binary);
    INT64Index index(f, false);
    assertSystem(index.search_node(key) == value, "Error!");
  }
  auto t3 = std::chrono::high_resolution_clock::now();
  auto duration = (t2 - t1);
  auto per_record = (t2 - t1) / (float) num_records;
  std::cout << 
    "insertion: " << std::chrono::duration_cast<std::chrono::seconds>( duration ).count() << "seconds (" <<
    std::chrono::duration_cast<std::chrono::microseconds>( per_record ).count() << "micros per record)" <<
    "\nsearch: " << std::chrono::duration_cast<std::chrono::microseconds>( t3 - t2 ).count() << "micro seconds";
}