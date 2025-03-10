#include <gtest/gtest.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <pmgdlib_gl.h>

using namespace pmgd;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

#define GTEST_COUT std::cerr << "[          ] [ INFO ]"

class TestContext {
  public:
  SDL_Window * window;
  SDL_GLContext gl_context;

  TestContext(){
    window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    gl_context = SDL_GL_CreateContext(window);

    // Alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    //glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA );
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0);

    // Depth
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0, 1.0);

    // clear screen
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClearColor(0., 0.5, 0.5, 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    SDL_GL_SwapWindow(window);
    glClearColor(0., 0.5, 0.5, 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
  }

  ~TestContext(){
    // TODO
  }
};

class TestSuite : public testing::Test {
  /// https://github.com/google/googletest/blob/main/docs/advanced.md#sharing-resources-between-tests-in-the-same-test-suite
 protected:
  // Per-test-suite set-up.
  // Called before the first test in this test suite.
  // Can be omitted if not needed.
  static void SetUpTestSuite() {
    shared_resource_ = new TestContext();

    // If `shared_resource_` is **not deleted** in `TearDownTestSuite()`,
    // reallocation should be prevented because `SetUpTestSuite()` may be called
    // in subclasses of FooTest and lead to memory leak.
    //
    // if (shared_resource_ == nullptr) {
    //   shared_resource_ = new ...;
    // }
  }

  // Per-test-suite tear-down.
  // Called after the last test in this test suite.
  // Can be omitted if not needed.
  static void TearDownTestSuite() {
    delete shared_resource_;
    shared_resource_ = nullptr;
  }

  // You can define per-test set-up logic as usual.
  // void SetUp() override { ... }

  // You can define per-test tear-down logic as usual.
  // void TearDown() override { ... }

  // Some expensive resource shared by all tests.
  public:
  static TestContext* shared_resource_;
};

TestContext* TestSuite::shared_resource_ = nullptr;

TEST_F(TestSuite, draw_texture) {
  GTEST_COUT << "See test image" << std::endl;
  std::shared_ptr<Image> image = get_test_image();
  TextureDrawer td(image);

  float size_x = SCREEN_WIDTH / 2 / float(SCREEN_WIDTH);
  float size_y = SCREEN_HEIGHT / 2 / float(SCREEN_HEIGHT);
  TextureDrawData data(v2(0,0), v2(size_x,size_y));

  for(int i = 0; i < 100; i++){
    SDL_GL_SwapWindow(TestSuite::shared_resource_->window);
    glClearColor(0., 0.5, 0.5, 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    // draw quad
    data.angle = 316*i/100;
    td.Draw(data);

    SDL_Delay(20);
  }
}

TEST_F(TestSuite, draw_image) {
  GTEST_COUT << "See lot of white squares" << std::endl;
  float vertices[10*12];
  for(int i = 0; i < 100; i++){
    SDL_GL_SwapWindow(TestSuite::shared_resource_->window);
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
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
