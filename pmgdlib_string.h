// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_STRING_HH
#define PMGDLIB_STRING_HH 1

namespace pmgd {

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
  void split_string(std::string &str, std::vector<std::string> &answer, const std::string &sep = " "){
    size_t sep_size = sep.size();
    size_t i_start = 0, i_end = 0;
    for(size_t i_max = str.size(); i_end + sep_size < i_max;){
      if(sep == str.substr(i_end, sep_size)){
        answer.push_back(str.substr(i_start, i_end-i_start));
        i_end = (i_start = i_end + sep_size);
      }
      else i_end++;
    }
    answer.push_back( str.substr(i_start, std::string::npos) );
  }

};

#endif
