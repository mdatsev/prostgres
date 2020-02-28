#include <iostream>
#include <type_traits>
#include <variant>

int main() {
   std::cout << std::is_trivially_copyable<std::variant<int64_t>>::value << '\n';
}