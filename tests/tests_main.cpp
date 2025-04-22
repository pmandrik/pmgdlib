#include <gtest/gtest.h>
#include "tests_msg.h"
#include "tests_string.h"
#include "tests_time.h"
#include "tests_pipeline.h"
#include "tests_config.h"

#define USE_STB
#include "tests_data.h"

using namespace pmgd;
using namespace std;

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
