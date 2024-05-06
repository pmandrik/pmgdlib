// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef TEST_STRING_HH
#define TEST_STRING_HH 1

#include "pmgdlib_string.h"

using namespace pmgd;

TEST(pmlib_string, strip) {
  std::string txt = "     ban an a     ";
  lstrip(txt);
  EXPECT_EQ(txt, "ban an a     ");
  rstrip(txt);
  EXPECT_EQ(txt, "ban an a");

  std::string txt_2 = "     c oc  oso     ";
  strip(txt_2);
  EXPECT_EQ(txt_2, "c oc  oso");
}

TEST(pmlib_string, split) {
  std::string txt = "1 22 333 4444 55555 666666";
  std::vector<std::string> parts;
  split_string(txt, parts);
  for(size_t i = 0; i < parts.size(); i++){
    EXPECT_EQ(parts.at(i).size(), i+1);
  }
  parts.clear();

  txt = "1 22 333 4444 55555 666666";
  split_string(txt, parts, "4");
  EXPECT_EQ(parts.at(0), "1 22 333 ");
  EXPECT_EQ(parts.at(1), "");
  EXPECT_EQ(parts.at(2), "");
  EXPECT_EQ(parts.at(3), "");
  EXPECT_EQ(parts.at(4), " 55555 666666");
  parts.clear();

  txt = "111";
  split_string(txt, parts, "11");
  EXPECT_EQ(parts.at(0), "");
  EXPECT_EQ(parts.at(1), "1");
  parts.clear();

  txt = "1 2 1";
  split_string_strip(txt, parts, "2");
  EXPECT_EQ(parts.at(0), "1");
  EXPECT_EQ(parts.at(1), "1");
}

TEST(pmlib_string, replace) {
  std::string txt = "12121";
  EXPECT_EQ(replace_all(txt, "2", "33"), 2);
  EXPECT_EQ(txt, "1331331");

  EXPECT_EQ(replace_all(txt, "1331331", "1"), 1);
  EXPECT_EQ(txt, "1");
}

TEST(pmlib_string, to_string_with_leading_zeros) {
  EXPECT_EQ(to_string_with_leading_zeros(1999, 8), "00001999");
  EXPECT_EQ(to_string_with_leading_zeros(1999, 4), "1999");
  EXPECT_EQ(to_string_with_leading_zeros(1999, 2), "19");
  EXPECT_EQ(to_string_with_leading_zeros(1999, 0), "");
  EXPECT_EQ(to_string_with_leading_zeros(-1999, 8), "-00001999");
}

TEST(pmlib_string, to_upper) {
  std::string txt = "abc";
  to_upper(txt);
  EXPECT_EQ(txt, "ABC");

  txt = "a1b1c";
  to_upper(txt);
  EXPECT_EQ(txt, "A1B1C");
}

TEST(pmlib_string, bool_from_string) {
  EXPECT_EQ(bool_from_string("True"), true);
  EXPECT_EQ(bool_from_string("1"), true);
  EXPECT_EQ(bool_from_string("on"), true);
  EXPECT_EQ(bool_from_string("off"), false);
  EXPECT_EQ(bool_from_string("NULL"), false);
}

#endif
