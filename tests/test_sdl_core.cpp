#include <test_common.h>

#include <SDL.h>
#include <SDL_opengl.h>

#define USE_SDL
#include "pmgdlib_core.h"
#include "pmgdlib_factory.h"

using namespace pmgd;

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

TEST(sdl_core, window) {
  int SCREEN_WIDTH = 600;
  int SCREEN_HEIGHT = 480;
    
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window * window = SDL_CreateWindow("TEST(sdl_core, window)",
                                      SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      SCREEN_WIDTH, SCREEN_HEIGHT,
                                      SDL_WINDOW_SHOWN);

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  EXPECT_TRUE(renderer != NULL);

  SDL_Rect squareRect;
  squareRect.w = std::min(SCREEN_WIDTH, SCREEN_HEIGHT) / 2;
  squareRect.h = std::min(SCREEN_WIDTH, SCREEN_HEIGHT) / 2;
  squareRect.x = SCREEN_WIDTH / 2 - squareRect.w / 2;
  squareRect.y = SCREEN_HEIGHT / 2 - squareRect.h / 2;

  for(int i = 0; i < 100; i++){
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &squareRect);
    SDL_RenderPresent(renderer);
    SDL_Delay(20);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
