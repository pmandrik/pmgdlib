#include <gtest/gtest.h>
#include "tests_string.h"
#include "tests_time.h"
#include "tests_pipeline.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
