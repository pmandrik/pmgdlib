#include <test_common.h>

#include <SDL.h>
#include <SDL_vulkan.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

TEST(sdl_native, init_gl_context) {
  // init
  if (SDL_Init( SDL_INIT_VIDEO ) < 0)
    FAIL() << "SDL_Init failed: " << SDL_GetError();

  SDL_Window * window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_VULKAN);

  if(window == NULL){
    FAIL() << "SDL_CreateWindow failed: %s" << SDL_GetError();
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
