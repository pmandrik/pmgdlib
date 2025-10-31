// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_STRING_HH
#define PMGDLIB_STRING_HH 1

#include <string>
#include <vector>

namespace pmgd {

  // General functions ============================================================================================================================
  //! remove whitespaces at the begin of the string
  void lstrip(std::string &s);

  //! remove whitespaces at the end of the string
  void rstrip(std::string &s);

  //! remove whitespaces at the end of the string
  void strip(std::string &s);

  //! split string by sep[arator] into answer vector
  void split_string(const std::string &str, std::vector<std::string> &answer, std::string sep = " ");

  //! strip every separated part of the string
  void split_string_strip(const std::string &str, std::vector<std::string> & answer, std::string sep = " ");

  //! join string
  std::string join_strings(const std::vector<std::string> &strings, std::string sep = "");

  std::string join_string_ptrs(const std::vector<std::string*> &strings, std::string sep = "");

  //! replace all substrings `from` to substring `to` and return number of replacements
  int replace_all(std::string & str, const std::string& from, const std::string& to);

  //! to_string value and add leading symbol e.g. to_string_with_leading(1999, 8, '0') -> "00001999"
  std::string to_string_with_leading(int integer, unsigned int N_symbols, const char symbol);

  //! 'abc' -> 'ABC'
  void to_upper(std::string & str);

  bool bool_from_string(std::string val, bool def_answer = true);

  // Special functions ============================================================================================================================
  std::string quote(const std::string & str, std::string qt = "\"");

  std::string quotec(const std::string & str, std::string qt = "\"");
};

#endif
