#include <gtest/gtest.h>

#include <SDL.h>
#include <SDL_opengl.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

#define GTEST_COUT std::cerr << "[          ] [ INFO ]"

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

  GTEST_COUT << "See white square" << std::endl;
  for(int i = 0; i < 100; i++){
    SDL_GL_SwapWindow(window);

    // draw quad
    float pos_x = -1. + 2. / 100. * i;
    float pos_y = -1. + 2. / 100. * i;
    float size_x = SCREEN_WIDTH / 6 / float(SCREEN_WIDTH);
    float size_y = SCREEN_HEIGHT / 6 / float(SCREEN_HEIGHT);

    glColor3f(1.f, 1.f, 1.f);

    glBegin(GL_TRIANGLES);
    glVertex3f(pos_x - size_x, pos_y - size_y, 0);
    glVertex3f(pos_x - size_x, pos_y + size_y, 0);
    glVertex3f(pos_x + size_x, pos_y - size_y, 0);
    glEnd();

    SDL_Delay(20);
  }

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

  GTEST_COUT << "See texture" << std::endl;
  //create test checker image
  unsigned char texDat[64];
  for (int i = 0; i < 64; ++i)
      texDat[i] = ((i + (i / 8)) % 2) * 128 + 127;

  //upload to GPU texture
  // TODO move to native test
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 8, 8, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texDat);
  glBindTexture(GL_TEXTURE_2D, 0);

  //match projection to window resolution (could be in reshape callback)
  glMatrixMode(GL_PROJECTION);
  glOrtho(0, 800, 0, 600, -1, 1);
  glMatrixMode(GL_MODELVIEW);

  //clear and draw quad with texture (could be in display callback)
  glClear(GL_COLOR_BUFFER_BIT);
  glBindTexture(GL_TEXTURE_2D, tex);
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glTexCoord2i(0, 0); glVertex2i(100, 100);
  glTexCoord2i(0, 1); glVertex2i(100, 500);
  glTexCoord2i(1, 1); glVertex2i(500, 500);
  glTexCoord2i(1, 0); glVertex2i(500, 100);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  for(int i = 0; i < 100; i++){
    SDL_GL_SwapWindow(window);
    glClearColor(0., 0.5, 0.5, 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, tex);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(100, 100);
    glTexCoord2i(0, 1); glVertex2i(100, 500);
    glTexCoord2i(1, 1); glVertex2i(500, 500);
    glTexCoord2i(1, 0); glVertex2i(500, 100);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_Delay(20);
  }

  SDL_Delay(2000);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
