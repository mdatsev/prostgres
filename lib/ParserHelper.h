#pragma once

bool is_word_char (char c);
bool is_digit (char c);
bool is_word_begin_char (char c);
bool is_token_char (char c);
bool is_whitespace (char c);
bool is_delim (char c);
class ParserHelper
{
public:
  ParserHelper (std::string input, std::size_t const curr_pos = 0);
  std::string get_word ();
  std::string get_token ();
  std::string get_literal ();
private:
  std::string input;
  std::size_t curr_pos;
};