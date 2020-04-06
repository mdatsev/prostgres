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
  int num_records = 10000000;
  int key, value;
  auto t1 = std::chrono::high_resolution_clock::now();
  {
    INT64Index index("/tmp/testfile", true);

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
    INT64Index index("/tmp/testfile", false);
    assertSystem(index.search_node(key, true) == value, "Error!");
  }
  auto t3 = std::chrono::high_resolution_clock::now();
  auto duration = (t2 - t1);
  auto per_record = (t2 - t1) / (float) num_records;
  std::cout << 
    "insertion: " << std::chrono::duration_cast<std::chrono::milliseconds>( duration ).count() << "seconds (" <<
    std::chrono::duration_cast<std::chrono::microseconds>( per_record ).count() << "micros per record)" <<
    "\nsearch: " << std::chrono::duration_cast<std::chrono::microseconds>( t3 - t2 ).count() << "micro seconds\n";
}