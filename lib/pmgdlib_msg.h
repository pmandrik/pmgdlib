// P.~Mandrik, 2025

#ifndef PMLIB_MSG_HH
#define PMLIB_MSG_HH 1

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#ifdef USE_SOURCE_TRACE
  #include <source_location>
#endif

namespace pmgd {
  enum verbose {
    SILENCE=0,
    ERROR,
    WARNING,
    INFO,
    DEBUG=4,
    VERBOSE
  };

  int & msg_verbose_lvl(){
    static int def_verbose_lvl = verbose::DEBUG;
    return def_verbose_lvl;
  };

  #define MSG_COLOR_RED     "\x1b[31m"
  #define MSG_COLOR_GREEN   "\x1b[32m"
  #define MSG_COLOR_YELLOW  "\x1b[33m"
  #define MSG_COLOR_BLUE    "\x1b[34m"
  #define MSG_COLOR_MAGENTA "\x1b[35m"
  #define MSG_COLOR_CYAN    "\x1b[36m"
  #define MSG_COLOR_RESET   "\x1b[0m"

  #define MSG_ERROR_PREFIX MSG_COLOR_RED "ERROR:" MSG_COLOR_RESET
  #define MSG_WARNING_PREFIX MSG_COLOR_YELLOW "WARNING:" MSG_COLOR_RESET
  #define MSG_INFO_PREFIX "INFO:"
  #define MSG_DEBUG_PREFIX MSG_COLOR_BLUE "DEBUG:" MSG_COLOR_RESET
  #define MSG_VERBOSE_PREFIX MSG_COLOR_CYAN "VERBOSE:" MSG_COLOR_RESET

  #ifdef USE_SOURCE_TRACE
    std::string msg_trace(const std::source_location& location = std::source_location::current()){
      return std::string(location.file_name()) + ":" + std::to_string(location.line()) + ":" + location.function_name() + ":";
    }
  #else
    std::string msg_trace(){
      return std::string(__FUNCTION__) + ":";
    }
  #endif

  std::string msg_short_trace(const std::source_location& location = std::source_location::current()){
    return std::string(location.file_name()) + ":" + std::to_string(location.line()) + ":";
  }

  #define msg_error(...)   if(verbose_lvl >= pmgd::verbose::ERROR)   pmgd::msg_err(MSG_ERROR_PREFIX, msg_trace(), __VA_ARGS__)
  #define msg_warning(...) if(verbose_lvl >= pmgd::verbose::WARNING) pmgd::msg_err(MSG_WARNING_PREFIX, msg_trace(), __VA_ARGS__)
  #define msg_info(...)    if(verbose_lvl >= pmgd::verbose::INFO)    pmgd::msg(MSG_INFO_PREFIX, msg_trace(), __VA_ARGS__)
  #define msg_debug(...)   if(verbose_lvl >= pmgd::verbose::DEBUG)   pmgd::msg(MSG_DEBUG_PREFIX, msg_trace(), __VA_ARGS__)
  #define msg_verbose(...) if(verbose_lvl >= pmgd::verbose::VERBOSE) pmgd::msg(MSG_VERBOSE_PREFIX, msg_trace(), __VA_ARGS__)

  #define msg_st_error(...)   if(verbose_lvl >= pmgd::verbose::ERROR)   pmgd::msg_err(MSG_ERROR_PREFIX, msg_short_trace(), __VA_ARGS__)
  #define msg_st_warning(...) if(verbose_lvl >= pmgd::verbose::WARNING) pmgd::msg_err(MSG_WARNING_PREFIX, msg_short_trace(), __VA_ARGS__)
  #define msg_st_info(...)    if(verbose_lvl >= pmgd::verbose::INFO)    pmgd::msg(MSG_INFO_PREFIX, msg_short_trace(), __VA_ARGS__)
  #define msg_st_debug(...)   if(verbose_lvl >= pmgd::verbose::DEBUG)   pmgd::msg(MSG_DEBUG_PREFIX, msg_short_trace(), __VA_ARGS__)
  #define msg_st_verbose(...) if(verbose_lvl >= pmgd::verbose::VERBOSE) pmgd::msg(MSG_VERBOSE_PREFIX, msg_short_trace(), __VA_ARGS__)

  #define msg_nt_error(...)   if(verbose_lvl >= pmgd::verbose::ERROR)   pmgd::msg_err(MSG_ERROR_PREFIX, __VA_ARGS__)
  #define msg_nt_warning(...) if(verbose_lvl >= pmgd::verbose::WARNING) pmgd::msg_err(MSG_WARNING_PREFIX, __VA_ARGS__)
  #define msg_nt_info(...)    if(verbose_lvl >= pmgd::verbose::INFO)    pmgd::msg(MSG_INFO_PREFIX, __VA_ARGS__)
  #define msg_nt_debug(...)   if(verbose_lvl >= pmgd::verbose::DEBUG)   pmgd::msg(MSG_DEBUG_PREFIX, __VA_ARGS__)
  #define msg_nt_verbose(...) if(verbose_lvl >= pmgd::verbose::VERBOSE) pmgd::msg(MSG_VERBOSE_PREFIX, __VA_ARGS__)

  // ======= msg ====================================================================
  void msg(){ std::cout << std::endl; };
  template<typename T> void msg(T t) { std::cout << t << std::endl; }
  template<typename T, typename... Args> void msg(T t, Args... args){
    std::cout << t << " ";
    msg(args...);
  }

  void msg_nll(){};
  template<typename T> void msg_nll(T t) { std::cout << t; }
  template<typename T, typename... Args> void msg_nll(T t, Args... args){
    std::cout << t << " ";
    msg_nll(args...);
  }

  template<typename T> void msg_sep(const std::string & separator, const std::string & last_symbol, T t){ std::cout << t << last_symbol; };
  template<typename T, typename... Args> void msg_sep(const std::string & separator, const std::string & last_symbol, T t, Args... args){
    std::cout << t << separator;
    msg_sep(separator, last_symbol, args...);
  }

  // ======= msg_err ====================================================================
  void msg_err(){ std::cerr << std::endl; };
  template<typename T> void msg_err(const T & t) { std::cerr << t << std::endl; }
  template<typename T, typename... Args> void msg_err(const T & t, Args... args){
    std::cerr << t << " ";
    msg_err(args...);
  }

  void msg_err_nll(){};
  template<typename T> void msg_err_nll(T t) { std::cerr << t << std::endl; }
  template<typename T, typename... Args> void msg_err_nll(T t, Args... args){
    std::cerr << t << " ";
    msg_err_nll(args...);
  }

  template<typename T> void msg_sep_err(const std::string & separator, const std::string & last_symbol, T t){ std::cerr << t << last_symbol; };
  template<typename T, typename... Args> void msg_sep_err(const std::string & separator, const std::string & last_symbol, T t, Args... args){
    std::cerr << t << separator;
    msg_sep_err(separator, last_symbol, args...);
  }

  class BaseMsg {
    public:
    int verbose_lvl = msg_verbose_lvl();

    template<typename... Args>
    bool CWM(bool cond, Args... args){
      if(cond) msg_nt_warning(args...);
      return cond;
    }
  };
};

#endif


















