#include <string>

#include "ParserHelper.h"

constexpr const char DELIM = ';';

bool is_word_char(char c) {
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
         (c >= '0' && c <= '9') ||
         c == '_';
}
bool is_digit(char c) {
  return (c >= '0' && c <= '9');
}
bool is_word_begin_char(char c) {
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
         c == '_';
}
bool is_token_char(char c) {
  return c == '(' ||
         c == ',' || 
         c == ')' ||
         c == '=' ||
         c == '<' ||
         c == '>' ||
         c == '!';
}
bool is_whitespace(char c) {
  return c == ' ' || c == '\t' || c == '\n';
}
bool is_delim(char c) {
  return c == ';';
}
ParserHelper::ParserHelper(std::string input, std::size_t const curr_pos)
    : input(input), curr_pos(curr_pos) {
}
std::string ParserHelper::get_word() {
  std::size_t start = std::string::npos;
  std::size_t end = std::string::npos;
  for (auto i = curr_pos; i < input.size(); i++) {
    if (start == std::string::npos) {
      // std::cout << "BEGIN?:" << input[i] << std::endl;
      if (is_word_begin_char(input[i])) {
        // std::cout << "BEGIN:" << input[i] << std::endl;
        start = i;
      } else if (is_whitespace(input[i])) {
        continue;
      } else {
        break;
      }
    } else {
      // std::cout << "END?:" << input[i] << std::endl;
      if (!is_word_char(input[i])) {
        // std::cout << "END:" << input[i] << std::endl;
        end = i;
        break;
      }
    }
  }
  if (start == std::string::npos) {
    // fail to find
    return {};
  }

  curr_pos = end;
  return input.substr(start, end - start);
}
std::string ParserHelper::get_token() {
  for (auto i = curr_pos; i < input.size() && !is_delim(input[i]); i++) {
    if (is_token_char(input[i])) {
      curr_pos = i + 1;
      return input.substr(i, 1);
    } else if (is_whitespace(input[i])) {
      continue;
    } else {
      break;
    }
  }
  // fail to find
  return {};
}
std::string ParserHelper::get_literal() {
  std::size_t start = std::string::npos;
  std::size_t end = std::string::npos;
  bool numeric;
  for (auto i = curr_pos; i < input.size(); i++) {
    if (start == std::string::npos) {
      if (is_digit(input[i]) || input[i] == '-') {
        numeric = true;
        start = i;
      } else if (input[i] == '\'') {
        numeric = false;
        start = i + 1;
      } else if (is_whitespace(input[i])) {
        continue;
      } else {
        break;
      }
    } else {
      if (numeric && !is_digit(input[i])) {
        end = i;
        break;
      } else if (!numeric && input[i] == '\'') {
        end = i;
        break;
      }
    }
  }
  if (start == std::string::npos) {
    // fail to find
    return {};
  }
  if (numeric) {
    curr_pos = end;
  } else {
    curr_pos = end + 1;
  }
  return input.substr(start, end - start);
}