// P.~Mandrik, 2025

#include "pmgdlib_msg.h"

#ifdef USE_SOURCE_TRACE
  #include <source_location>
#endif

namespace pmgd {
  int & msg_verbose_lvl(){
    static int def_verbose_lvl = verbose::DEBUG;
    return def_verbose_lvl;
  };

  #ifdef USE_SOURCE_TRACE
    std::string msg_trace(const std::source_location& location = std::source_location::current()){
      return std::string(location.file_name()) + ":" + std::to_string(location.line()) + ":" + location.function_name() + ":";
    }
    std::string msg_short_trace(const std::source_location& location = std::source_location::current()){
      return std::string(location.file_name()) + ":" + std::to_string(location.line()) + ":";
    }
  #else
    std::string msg_trace(){
      return std::string(__FUNCTION__) + ":";
    }
    std::string msg_short_trace(){
      return std::string(__FUNCTION__) + ":";
    }
  #endif
  
  // ======= msg ====================================================================
  void msg(){ std::cout << std::endl; };
  void msg_nll(){};

  // ======= msg_err ====================================================================
  void msg_err(){ std::cerr << std::endl; };
  void msg_err_nll(){};
}


















