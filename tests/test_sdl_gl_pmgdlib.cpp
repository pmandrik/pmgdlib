#include <gtest/gtest.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <pmgdlib_gl.h>

using namespace pmgd;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

#define GTEST_COUT std::cerr << "[          ] [ INFO ]"

void create_window(){
}

TEST(sdl_native, init_gl_context) {
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

  GTEST_COUT << "See lot of white squares" << std::endl;
  float vertices[10*12];
  for(int i = 0; i < 100; i++){
    SDL_GL_SwapWindow(window);
    glClearColor(0., 0.5, 0.5, 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    // draw quad
    glColor3f(1.f, 1.f, 1.f);

    float size_x = SCREEN_WIDTH / 20 / float(SCREEN_WIDTH);
    float size_y = SCREEN_HEIGHT / 20 / float(SCREEN_HEIGHT);

    for(int qi = 0; qi < 10; qi++){
      float pos_x = -0.8 + 0.1 * qi;
      float pos_y = -0.8 + 0.1 * qi;
      v2 pos = v2(pos_x * cos(3.14 * qi + 0.1*i), pos_y * sin(3.14 * qi + (0.05+0.02*qi)*i));
      v2 size = v2(size_x, size_y);
      fill_verices_array(vertices, qi*12, pos, size, 0);
    }

    draw_quad_array(vertices, 10);

    SDL_Delay(20);
  }
  return;

  GTEST_COUT << "See white square" << std::endl;
  for(int i = 0; i < 100; i++){
    SDL_GL_SwapWindow(window);

    // draw quad
    float pos_x = -1. + 2. / 100. * i;
    float pos_y = -1. + 2. / 100. * i;
    float size_x = SCREEN_WIDTH / 6 / float(SCREEN_WIDTH);
    float size_y = SCREEN_HEIGHT / 6 / float(SCREEN_HEIGHT);

    glColor3f(1.f, 1.f, 1.f);

    draw_quad(v2(pos_x, pos_y), v2(size_x, size_y), 360 * i / 100);

    SDL_Delay(20);
  }
  return;

  for(int i = 0; i < 100; i++){
    SDL_GL_SwapWindow(window);
    glClearColor(0., 0.5, 0.5, 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    // draw quad
    float pos_x = 1. - 2. / 100. * i;
    float pos_y = 1. - 2. / 100. * i;
    float size_x = SCREEN_WIDTH / 6 / float(SCREEN_WIDTH);
    float size_y = SCREEN_HEIGHT / 6 / float(SCREEN_HEIGHT);

    glColor3f(1.f, 1.f, 1.f);

    glBegin(GL_TRIANGLES);
    glVertex3f(pos_x - size_x, pos_y - size_y, 0);
    glVertex3f(pos_x - size_x, pos_y + size_y, 0);
    glVertex3f(pos_x + size_x, pos_y - size_y, 0);

    glVertex3f(pos_x + size_x, pos_y - size_y, 0);
    glVertex3f(pos_x + size_x, pos_y + size_y, 0);
    glVertex3f(pos_x - size_x, pos_y + size_y, 0);
    glEnd();

    SDL_Delay(20);
  }

  SDL_Delay(2000);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
