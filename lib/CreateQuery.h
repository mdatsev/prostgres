#pragma once

#include "DBTypes.h"

class CreateQuery
{
public:
  std::string table_name;
  std::vector <Field> fields;
  static CreateQuery parse (std::string input);
private:
  CreateQuery (std::string input);
};