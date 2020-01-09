#include <stdexcept>

#include "errors.h"

void assertUser (bool const cond, char const * message)
                                                       {
  if (!cond) {
    throw UserError(message);
  }
}
