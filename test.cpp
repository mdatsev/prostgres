#include <iostream>
#include <type_traits>
#include <variant>
#include <filesystem>
#include <fstream>

int main() {
  auto mode = std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary;
  auto data_path = std::filesystem::path("./test");
  std::fstream data_file;
  data_file.open(data_path, mode);
  data_file.seekg(0, std::ios::end);
  char value[4]{};
  data_file.write(value, 4);
  data_file.seekg(0, std::ios::end);
  std::cout << data_file.tellg();
}