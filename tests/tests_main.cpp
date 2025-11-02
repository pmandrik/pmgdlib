#include <test_common.h>

#include "tests_math.h"
#include "tests_msg.h"
#include "tests_string.h"
#include "tests_time.h"
#include "tests_pipeline.h"
#include "tests_config.h"
#include "tests_core.h"

#include "tests_data.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
