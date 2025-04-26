#include <test_common.h>

#include <SDL.h>
#include <SDL_opengl.h>

#define USE_SDL
#include "pmgdlib_factory.h"

TEST(sdl_core, mouse) {
}

TEST(sdl_core, keyboard) {
}

TEST(pmlib_data, load_txt) {
  std::string path = "../data/test.txt";
  SysOptions bo;
  bo.io = "SDL";
  Backend bk = get_backend(bo);
  std::string txt_data = bk.io->ReadTxt(path);
  EXPECT_EQ(txt_data, "test data");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
