#include <gtest/gtest.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <pmgdlib_gl.h>
#include <vector>

using namespace pmgd;
using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

#define GTEST_COUT std::cerr << "[          ] [ INFO ]"

const std::string vert = R"(
    #version 300 es
    in vec3 in_Position;
    in vec2 a_texCoord;
    out vec2 v_texCoord;

    void main(void) {
      gl_Position = vec4(in_Position, 1.0);
      v_texCoord = a_texCoord;
    }
  )";

const std::string frag = R"(
      #version 300 es
      precision mediump float;
      in vec2 v_texCoord;
      uniform sampler2D text_0;
      out vec4 fragColor;
      void main(){
        fragColor = texture2D(text_0, v_texCoord);
      }
    )";

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

TEST_F(TestSuite, quad_array) {
  GTEST_COUT << "See many texture images" << std::endl;
  std::shared_ptr<Image> image = get_test_image();
  float size_x = SCREEN_WIDTH / 20 / float(SCREEN_WIDTH);
  float size_y = SCREEN_HEIGHT / 20 / float(SCREEN_HEIGHT);
  TextureGl td(image);


  ShaderGl shader;
  shader.LoadVert(vert);
  shader.LoadFrag(frag);
  shader.CreateProgram();

  int n_quads = 30;
  QuadsArrayGl qad(n_quads);

  vector<unsigned int> ids;
  vector<TextureDrawData*> quads;
  vector<v2> directions;
  for(int i = 0; i < n_quads; i ++){
    TextureDrawData* data = new TextureDrawData(v2(0,0), v2(size_x,size_y));
    float width = (rand() + 100) % 1000 / 1000.;
    if(width > 1.) width = 0.9;
    data->tpos = v2(0,0);
    data->tsize = v2(width, width);
    quads.push_back(data);
    ids.push_back(qad.Add(data));
    directions.push_back(v2(1,0).Rotated(rand()%360));
  }

  return;
  for(int i = 0; i < 100; i++){
    SDL_GL_SwapWindow(TestSuite::shared_resource_->window);
    glClearColor(0., 0.5, 0.5, 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    continue;
    for(int qi = 0; qi < n_quads; qi ++){
      TextureDrawData* quad = quads[qi];
      v2 & speed = directions[qi];
      quad->pos += speed;
      if(quad->pos.x < -SCREEN_WIDTH) quad->pos.x = SCREEN_WIDTH;
      if(quad->pos.x >  SCREEN_WIDTH) quad->pos.x = -SCREEN_WIDTH;
      if(quad->pos.y < -SCREEN_HEIGHT) quad->pos.y = SCREEN_HEIGHT;
      if(quad->pos.y >  SCREEN_HEIGHT) quad->pos.y = -SCREEN_HEIGHT;
      qad.SetPos(ids[qi], quad);
    }

    // draw quads
    td.Bind();
    shader.Bind();
    GLint position = glGetUniformLocation(shader.program_id, "text_0");
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(position, 0);
    // qad.Draw();
    shader.Unbind();
    td.Unbind();
  }
}

TEST_F(TestSuite, draw_texture_atlas) {
  GTEST_COUT << "See many texture images" << std::endl;
  GTEST_COUT << "texture setup..." << std::endl;
  std::shared_ptr<Image> image = get_test_image();
  TextureGl td(image);
  TextureAtlas atlas;

  int n_quads = 30;

  float *data = new float[n_quads*4*5];
  int *indexes = fill_indexes_array(n_quads);
  float size_x = SCREEN_WIDTH / 20 / float(SCREEN_WIDTH);
  float size_y = SCREEN_HEIGHT / 20 / float(SCREEN_HEIGHT);

  for(int i = 0; i < n_quads; i ++){
    float pos_x = -0.8 + 0.1 * i;
    float pos_y = -0.8 + 0.1 * i;
    v2 pos = v2(pos_x * cos(3.14 * i + 0.1*i), pos_y * sin(3.14 * i + (0.05+0.02*i)*i));
    v2 size = v2(size_x, size_y);

    fill_verices_array(data, i*20, pos, size, 0, 5);

    float width = (rand() + 100) % 1000 / 1000.;
    if(width > 1.) width = 0.9;
    // width = 1;
    atlas.AddTexTile(std::to_string(i), v2(0,0), v2(width, width));

    TexTile * tile = atlas.GetTexTile(std::to_string(i));
    fill_texture_array(data, 20*i+3, tile->tpos, tile->tsize, 5);
  }

  GTEST_COUT << "shader setup..." << std::endl;
  ShaderGl shader;
  shader.LoadVert(vert);
  shader.LoadFrag(frag);
  shader.CreateProgram();

  GTEST_COUT << "loop..." << std::endl;
  for(int i = 0; i < 100; i++){
    SDL_GL_SwapWindow(TestSuite::shared_resource_->window);
    glClearColor(0., 0.5, 0.5, 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    for(int qi = 0; qi < n_quads; qi ++){
      v2 pos(0.8, 0);
      pos = pos.Rotated(360 / n_quads * qi + 0.2*i);
      v2 size = v2(size_x, size_y);
      fill_verices_array(data, qi*20, pos, size, 0, 5);
      int index = qi * (1 - ((i % n_quads) == qi));
      TexTile * tile = atlas.GetTexTile(std::to_string(index));
      fill_texture_array(data, 20*qi+3, tile->tpos, tile->tsize, 5);
    }

    // draw quads
    td.Bind();
    shader.Bind();
    GLint position = glGetUniformLocation(shader.program_id, "text_0");
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(position, 0);

    draw_textured_elements(n_quads*4, data, n_quads*6, indexes);
    shader.Unbind();
    td.Unbind();

    SDL_Delay(30);
  }

  delete[] indexes;
}

TEST_F(TestSuite, draw_texture) {
  GTEST_COUT << "See test image" << std::endl;
  std::shared_ptr<Image> image = get_test_image();
  TextureGl td(image);

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
    draw_textured_quad(td, data);

    SDL_Delay(20);
  }
}

TEST_F(TestSuite, draw_quads) {
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

TEST_F(TestSuite, invalid_parameters) {
  std::shared_ptr<Image> image = get_test_image();
  image->format = image_format::UNDEFINED - 1;
  TextureGl td1(image);
  EXPECT_EQ(td1.GetId(), 0);

  image = get_test_image();
  image->type = image_type::UNDEFINED - 1;
  TextureGl td2(image);
  EXPECT_EQ(td2.GetId(), 0);

  ShaderGl shader;
  EXPECT_EQ(shader.Load(2 + GL_VERTEX_SHADER + GL_FRAGMENT_SHADER, vert), PM_ERROR_INCORRECT_ARGUMENTS);
  EXPECT_EQ(shader.Load(3 + GL_VERTEX_SHADER + GL_FRAGMENT_SHADER, frag), PM_ERROR_INCORRECT_ARGUMENTS);
  EXPECT_EQ(shader.CreateProgram(), PM_ERROR_CLASS_ATTRIBUTES);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
