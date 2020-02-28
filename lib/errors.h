#pragma once

#include <stdexcept>

class UserError : std::logic_error {
 public:
  using std::logic_error::logic_error;
  using std::logic_error::what;
};
void assertUser(bool const cond, char const* message);

class SystemError : std::logic_error {
 public:
  using std::logic_error::logic_error;
  using std::logic_error::what;
};
void assertSystem(bool const cond, char const* message);