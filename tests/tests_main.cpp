#include <gtest/gtest.h>
#include "tests_msg.h"
#include "tests_string.h"
#include "tests_time.h"
#include "tests_pipeline.h"
#include "tests_config.h"

#include "pmgdlib_core.h"
#include "pmgdlib_factory.h"

using namespace pmgd;
using namespace std;

TEST(pmlib_data, load_txt) {
  BackendOptions bo;
  Backend bk = get_backend(bo);
  std::string txt_data = bk.io->ReadTxt("/any/path");
  EXPECT_EQ(txt_data, "dummy data");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
