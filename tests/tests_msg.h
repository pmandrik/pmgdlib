// P.~Mandrik, 2025

#ifndef TEST_MSG_HH
#define TEST_MSG_HH 1

#include "pmgdlib_msg.h"
#include <stdio.h>

using namespace pmgd;
using namespace std;

std::streambuf *old, *olderr;
std::stringstream buffer;

void stream_redirection_on(){
  old = std::cout.rdbuf(buffer.rdbuf());
  olderr = std::cerr.rdbuf(buffer.rdbuf());
}

void stream_redirection_off(){
  std::cout.rdbuf(old);
  std::cerr.rdbuf(olderr);
}

bool check_buffer(std::string txt){
  std::string buff = buffer.str();
  return strncmp(buff.c_str(), txt.c_str(), txt.size()) == 0;
}

bool check_buffer_empty(){
  std::string buff = buffer.str();
  return not buff.size();
}

void clear_buffer(){
  buffer.str("");
}

TEST(msg_test, def) {
  clear_buffer();
  std::string txt;
  txt = "simple msg";
  stream_redirection_on();
  msg_nll(txt);
  stream_redirection_off();
  EXPECT_TRUE(check_buffer(txt));

  clear_buffer();
  txt = "msg simple";
  stream_redirection_on();
  msg(txt);
  stream_redirection_off();
  EXPECT_TRUE(check_buffer(txt + "\n"));

  clear_buffer();
  txt = "msg error";
  stream_redirection_on();
  msg_err(txt);
  stream_redirection_off();
  EXPECT_TRUE(check_buffer(txt + "\n"));
}

void test_verbose_level(std::string txt, int verbose_lvl, int level, std::string prefix){
  txt = std::string(prefix) + " " + txt;
  if(verbose_lvl >= level) EXPECT_TRUE(check_buffer(txt));
  else EXPECT_TRUE(check_buffer_empty());
  clear_buffer();
}

TEST(msg_test, verbose_level) {
  const char error_prefix[] = MSG_ERROR_PREFIX;
  const char warn_prefix[] = MSG_WARNING_PREFIX;
  const char info_prefix[] = MSG_INFO_PREFIX;
  const char debug_prefix[] = MSG_DEBUG_PREFIX;
  const char verbose_prefix[] = MSG_VERBOSE_PREFIX;
  std::string header;

  std::string txt = "sample txt";
  for(int verbose_lvl = verbose::SILENCE; verbose_lvl <= verbose::VERBOSE; verbose_lvl++){
    clear_buffer();
    stream_redirection_on();
    msg_error(txt); header = " " + msg_trace();
    stream_redirection_off();
    test_verbose_level(txt, verbose_lvl, verbose::ERROR, error_prefix + header);

    txt += "1";
    clear_buffer();
    stream_redirection_on();
    msg_warning(txt); header = " " + msg_trace();
    stream_redirection_off();
    test_verbose_level(txt, verbose_lvl, verbose::WARNING, warn_prefix + header);

    txt += "2";
    clear_buffer();
    stream_redirection_on();
    msg_info(txt); header = " " + msg_trace();
    stream_redirection_off();
    test_verbose_level(txt, verbose_lvl, verbose::INFO, info_prefix + header);

    txt += "3";
    clear_buffer();
    stream_redirection_on();
    msg_debug(txt); header = " " + msg_trace();
    stream_redirection_off();
    test_verbose_level(txt, verbose_lvl, verbose::DEBUG, debug_prefix + header);

    txt += "4";
    clear_buffer();
    stream_redirection_on();
    msg_verbose(txt); header = " " + msg_trace();
    stream_redirection_off();
    test_verbose_level(txt, verbose_lvl, verbose::VERBOSE, verbose_prefix + header);
  }
}

TEST(msg_test, sep) {
  clear_buffer();
  stream_redirection_on();
  msg_err("a", "b", "c");
  stream_redirection_off();
  EXPECT_TRUE(check_buffer("a b c"));

  clear_buffer();
  stream_redirection_on();
  msg_err("a", 1, "c", 2, "b", 4);
  stream_redirection_off();
  EXPECT_TRUE(check_buffer("a 1 c 2 b 4"));

  clear_buffer();
  stream_redirection_on();
  msg_sep("%^$", "!!!", "c", 2, "b", 4);
  stream_redirection_off();
  EXPECT_TRUE(check_buffer("c%^$2%^$b%^$4!!!"));
}

#endif
