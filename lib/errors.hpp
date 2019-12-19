#pragma once

#include <stdexcept>

class UserError : std::logic_error {
 public:
  using std::logic_error::logic_error;
  using std::logic_error::what;
};

void assertUser (const bool cond, const char* message) {
  if (!cond) {
    throw UserError(message);
  }
}