#include <test_common.h>

#include <SDL.h>
#include <SDL_opengl.h>

#define USE_SDL
#include "pmgdlib_core.h"
#include "pmgdlib_factory.h"

using namespace pmgd;
int SCREEN_WIDTH = 600;
int SCREEN_HEIGHT = 480;

TEST(sdl_core, mouse) {
}

TEST(sdl_core, keyboard) {
}

TEST(pmlib_data, load_txt) {
  std::string path = "../data/test.txt";
  SysOptions bo;
  bo.io = "SDL";
  auto bk = get_backend(bo);
  std::string txt_data = bk->ReadTxt(path);
  EXPECT_EQ(txt_data, "test data");
}

class SdlTestContext {
  public:
  SDL_Window * window;
  SDL_Renderer *renderer;

  SdlTestContext(){
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("TEST(sdl_core, window)",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                SCREEN_WIDTH, SCREEN_HEIGHT,
                                SDL_WINDOW_SHOWN);


    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  }

  ~SdlTestContext(){
  }
};

class SdlTestSuite : public testing::Test {
  protected:
  virtual void SetUp() {
    shared_resource_ = new SdlTestContext();
  }

  static void TearDownTestSuite() {
    delete shared_resource_;
    shared_resource_ = nullptr;
  }

  public:
  static SdlTestContext* shared_resource_;
};

SdlTestContext* SdlTestSuite::shared_resource_ = nullptr;

void tick_renderer(SDL_Renderer *renderer, DrawerSDL * drawer){
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
    drawer->Draw();
    SDL_RenderPresent(renderer);
    SDL_Delay(20);
}

TEST_F(SdlTestSuite, drawer) {
  SDL_Renderer *renderer = SdlTestSuite::shared_resource_->renderer;

  SDL_Surface *bmpSurf = SDL_LoadBMP("../data/img.bmp");
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, bmpSurf);

  v2 sw = v2(SCREEN_WIDTH, SCREEN_HEIGHT) / 2;
  CameraSDL camera(sw);
  DrawerSDL drawer(renderer, &camera, texture);

  TextureDrawData tdd;
  tdd.size.x = std::min(SCREEN_WIDTH, SCREEN_HEIGHT) / 4;
  tdd.size.y = std::min(SCREEN_WIDTH, SCREEN_HEIGHT) / 4;
  tdd.pos = sw;
  tdd.tpos = v2();
  tdd.tsize = v2(bmpSurf->w, bmpSurf->h);
  drawer.Add(&tdd);

  TextureDrawData tdd2 = tdd;
  tdd2.pos += v2(tdd.size.x, tdd.size.x*0.5);
  drawer.Add(&tdd2);

  TextureDrawData tdd3 = tdd;
  tdd3.pos -= v2(tdd.size.x, tdd.size.x*0.5);
  drawer.Add(&tdd3);

  for(int i = 0; i < 100; i++){
    tick_renderer(renderer, &drawer);
    if(i < 50)
      tdd.pos.x += 1;
    else
      tdd.pos.y += 1;
  }
  tdd.pos = sw;

  for(int i = 0; i < 100; i++){
    tick_renderer(renderer, &drawer);
    if(i < 50)
      camera.pos.x += 1;
    else
      camera.pos.y += 1;
  }

  camera.pos = v2(0,0);
  for(int i = 0; i < 360; i++){
    tick_renderer(renderer, &drawer);
    camera.angle += 1;
  }

}

TEST_F(SdlTestSuite, window) {
  SDL_Renderer *renderer = SdlTestSuite::shared_resource_->renderer;

  EXPECT_TRUE(renderer != nullptr);
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
