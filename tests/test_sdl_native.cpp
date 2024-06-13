#include <gtest/gtest.h>

#include <SDL.h>
#include <SDL_opengl.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

#define GTEST_COUT std::cerr << "[          ] [ INFO ]"

TEST(sdl_native, init) {
  // init
  if (SDL_Init( SDL_INIT_VIDEO ) < 0)
    FAIL() << "SDL_Init failed: " << SDL_GetError();

  SDL_Window * window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);

  if(window == NULL){
    FAIL() << "SDL_CreateWindow failed: %s" << SDL_GetError();
  }

  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  if(gl_context == NULL){
    FAIL() << "SDL_GL_CreateContext failed: %s" << SDL_GetError();
  }

  // clear screen
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  glClearColor(0., 0.5, 0.5, 1.);
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);
  SDL_GL_SwapWindow(window);
  glClearColor(0., 0.5, 0.5, 1.);
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);

  GTEST_COUT << "See white square" << std::endl;
  for(int i = 0; i < 100; i++){
    SDL_GL_SwapWindow(window);

    // draw quad
    float pos_x = -1. + 2. / 100. * i;
    float pos_y = -1. + 2. / 100. * i;
    float size_x = SCREEN_WIDTH / 6 / float(SCREEN_WIDTH);
    float size_y = SCREEN_HEIGHT / 6 / float(SCREEN_HEIGHT);

    glColor3f(1.f, 1.f, 1.f);

    glBegin(GL_QUADS);
    glVertex3f(pos_x - size_x, pos_y + size_y, 0);
    glVertex3f(pos_x + size_x, pos_y + size_y, 0);
    glVertex3f(pos_x + size_x, pos_y - size_y, 0);
    glVertex3f(pos_x - size_x, pos_y - size_y, 0);
    glEnd();

    SDL_Delay(20);
  }

  for(int i = 0; i < 100; i++){
    SDL_GL_SwapWindow(window);
    glClearColor(0., 0.5, 0.5, 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    // draw quad
    float pos_x = -1. + 2. / 100. * i;
    float pos_y = -1. + 2. / 100. * i;
    float size_x = SCREEN_WIDTH / 6 / float(SCREEN_WIDTH);
    float size_y = SCREEN_HEIGHT / 6 / float(SCREEN_HEIGHT);

    glColor3f(1.f, 1.f, 1.f);

    glBegin(GL_QUADS);
    glVertex3f(pos_x - size_x, pos_y + size_y, 0);
    glVertex3f(pos_x + size_x, pos_y + size_y, 0);
    glVertex3f(pos_x + size_x, pos_y - size_y, 0);
    glVertex3f(pos_x - size_x, pos_y - size_y, 0);
    glEnd();

    SDL_Delay(20);
  }

  SDL_Delay(2000);
}



int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
