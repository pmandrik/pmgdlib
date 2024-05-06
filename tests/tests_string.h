// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef TEST_STRING_HH
#define TEST_STRING_HH 1

#include "pmgdlib_string.h"

using namespace pmgd;

TEST(pmlib_pipeline, strip) {
  std::string txt = "     ban an a     ";
  lstrip(txt);
  EXPECT_EQ(txt, "ban an a     ");
  rstrip(txt);
  EXPECT_EQ(txt, "ban an a");

  std::string txt_2 = "     c oc  oso     ";
  strip(txt_2);
  EXPECT_EQ(txt_2, "c oc  oso");
}

TEST(pmlib_pipeline, split) {
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

#endif
