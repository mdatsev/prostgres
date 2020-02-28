#include <iostream>
#include <fstream>
#include <filesystem>

#include "lib/DB.h"
#include "lib/CreateQuery.h"
#include "lib/InsertQuery.h"
#include "lib/SelectQuery.h"
#include "lib/errors.h"
#include "lib/ParserHelper.h"

int main (int argc, const char** argv) {
  DB db("./db");
  
  while (std::cin) {
    try {
      std::cout << "> "; 
      std::string buffer;
      const std::string& query_string = buffer;

      std::getline(std::cin, buffer, ';');
      if(std::cin.eof()) {
        break;
      }
      auto command = ParserHelper(query_string).get_word();
      if (command == "create") {
        db.execute(CreateQuery::parse(query_string));
      } else if (command == "insert") {
        db.execute(InsertQuery::parse(query_string));
      } else if (command == "select") {
        db.execute(SelectQuery::parse(query_string));
      } else {
        throw UserError("Invalid command");
      }
    } catch (UserError e) {
      std::cout << e.what() << ". Please check the manual :)\n" << std::endl;
    } catch (SystemError e) {
      std::cout << "System error. " << e.what() << std::endl;
    }
  }
}