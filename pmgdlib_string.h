// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_STRING_HH
#define PMGDLIB_STRING_HH 1

#include "pmgdlib_defs.h"

namespace pmgd {

  // General functions ============================================================================================================================
  //! remove whitespaces at the begin of the string
  inline void lstrip(std::string &s) {
    if(not s.size()) return;
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
  }

  //! remove whitespaces at the end of the string
  inline void rstrip(std::string &s) {
    if(not s.size()) return;
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(), s.end());
  }

  //! remove whitespaces at the end of the string
  inline void strip(std::string &s) {
    if(not s.size()) return;
    lstrip(s);
    rstrip(s);
  }

  //! split string by sep[arator] into answer vector
  void split_string(const std::string &str, std::vector<std::string> &answer, std::string sep = " "){
    size_t sep_size = sep.size();
    size_t i_start = 0, i_end = 0;
    for(size_t i_max = str.size(); i_end + sep_size <= i_max;){
      if(sep == str.substr(i_end, sep_size)){
        answer.push_back(str.substr(i_start, i_end-i_start));
        i_end = (i_start = i_end + sep_size);
      }
      else i_end++;
    }
    answer.push_back( str.substr(i_start, std::string::npos) );
  }

  //! strip every separated part of the string
  void split_string_strip(const std::string &str, std::vector<std::string> & answer, std::string sep = " "){
    split_string(str, answer, sep);
    for(size_t i = 0; i < answer.size(); ++i){
      strip(answer[i]);
    }
  }

  //! join string
  std::string join_strings(const std::vector<std::string> &strings, std::string sep = "") {
    std::string answer;
    for(size_t i = 0, i_max = strings.size(); i < i_max; ++i){
      answer += strings[i];
      if(i != i_max - 1) answer += sep;
    }
    return answer;
  }

  std::string join_string_ptrs(const std::vector<std::string*> &strings, std::string sep = "") {
    std::string answer;
    for(size_t i = 0, i_max = strings.size(); i < i_max; ++i){
      answer += *(strings[i]);
      if(i != i_max - 1) answer += sep;
    }
    return answer;
  }

  //! replace all substrings `from` to substring `to` and return number of replacements
  int replace_all(std::string & str, const std::string& from, const std::string& to) {
    int count = 0;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
        count++;
    }
    return count;
  }

  //! to_string value and add leading symbol e.g. to_string_with_leading(1999, 8, '0') -> "00001999"
  std::string to_string_with_leading(int integer, unsigned int N_symbols, const char symbol){
    std::string str = std::to_string( abs(integer) );
    if(N_symbols < str.size()) return str.substr(0, N_symbols);
    if(integer < 0)
      return std::string("-") + std::string(N_symbols - str.size(), symbol) + str;
    return std::string(N_symbols - str.size(), symbol) + str;
  }

  //! 'abc' -> 'ABC'
  void to_upper(std::string & str){
    for(size_t i=0; str[i]!='\0'; i++){
      if (str[i] >= 'a' && str[i] <= 'z')
        str[i] = str[i] - 32;
    }
  }

  bool bool_from_string(std::string val, bool def_answer = true){
    to_upper(val);
    if(not val.size() or val=="FALSE" or val=="NULL" or val=="NULLPTR" or val=="0" or val=="OFF") return false;
    if(val=="TRUE" or val=="1" or val=="ON") return true;
    return def_answer;
  }

  // Special functions ============================================================================================================================
  std::string quote(const std::string & str, std::string qt = "\""){
    return qt + str + qt;
  }

  std::string quotec(const std::string & str, std::string qt = "\""){
    return qt + str + qt + ",";
  }
};

#endif
