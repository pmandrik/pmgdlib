#include <test_common.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <pmgdlib_gl.h>
#include <vector>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

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

TEST_F(TestSuite, double_frame_buffer) {
  GTEST_COUT << "DoubleFrameBuffer test" << std::endl;
  float size_x = SCREEN_WIDTH / 2 / float(SCREEN_WIDTH);
  float size_y = SCREEN_HEIGHT / 2 / float(SCREEN_HEIGHT);

  std::shared_ptr<Image> image = get_test_image();
  TextureGl td(image);
  TextureDrawData image_data(v2(0,0), v2(size_x/2,size_y/2));

  int fb_size_x = 100;
  int fb_size_y = 100;
  std::shared_ptr<FrameBufferGL> fb1 = std::make_shared<FrameBufferGL>(fb_size_x, fb_size_y);
  std::shared_ptr<FrameBufferGL> fb2 = std::make_shared<FrameBufferGL>(fb_size_x, fb_size_y);
  fb1->Clear();
  fb2->Clear();
  DoubleFrameBuffer dfb(fb1, fb2);
  TextureDrawData dfb_data(v2(0,0), v2(1, 1));
  dfb_data.flip_y = true;
  TextureDrawData data(v2(0,0), v2(size_x,size_y));

  // draw image data once
  dfb.Target();
  draw_textured_quad(td, image_data);
  dfb.Untarget();

  // setup shader
  const std::string vert_fb = R"(
    #version 300 es
    in vec3 in_Position;
    in vec2 a_texCoord;
    out vec2 v_texCoord;

    void main(void) {
      gl_Position = vec4(in_Position, 1.0);
      v_texCoord = a_texCoord;
    }
  )";

  const std::string frag_uni = R"(
    #version 300 es
    precision mediump float;
    in vec2 v_texCoord;
    uniform float var0;
    uniform float var1;
    uniform sampler2D text_0;
    out vec4 fragColor;
    void main(){
      vec2 v_texCoord_u = v_texCoord + vec2(var1/100., 1./100.);
      vec2 v_texCoord_d = v_texCoord - vec2(1./100., var1/100.);

      fragColor = texture2D(text_0, v_texCoord);
      vec4 fragColor_u = texture2D(text_0, v_texCoord_u);
      vec4 fragColor_d = texture2D(text_0, v_texCoord_d);

      fragColor = var0 * fragColor + 0.5*(1. - var0) * (fragColor_u + fragColor_d);
      fragColor.a = 1.;
    }
  )";

  ShaderGl shader;
  shader.LoadVert(vert_fb);
  shader.LoadFrag(frag_uni);
  shader.CreateProgram();
  shader.AddUniform("text_0");
  shader.AddUniform("var0");
  shader.AddUniform("var1");

  QuadsArrayGl qad(1);
  qad.Add(&dfb_data);

  for(int i = 0; i < 300; i++){
    SDL_GL_SwapWindow(TestSuite::shared_resource_->window);
    glClearColor(0., 0.5, 0.5, 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);


    dfb.Flip();
    dfb.BindBackTexture();
    dfb.Target();
    if(i>30) {
      shader.Bind();
      shader.EnableTexture(shader.GetUniform("text_0"), 0);
      shader.UpdateUniform1f(shader.GetUniform("var0"), 0.96);
      shader.UpdateUniform1f(shader.GetUniform("var1"), 1 + 5 * cos(3.14 * i/50.));
      qad.Draw();
      shader.Unbind();
    } else {
      draw_dfb_back_quad(dfb, dfb_data);
    }
    dfb.Untarget();
    dfb.UnbindBackTexture();

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    draw_dfb_quad(dfb, data);

    SDL_Delay(20);
  }
  exit(0);
}

TEST_F(TestSuite, frame_buffer) {
  GTEST_COUT << "FrameBuffer test" << std::endl;
  float size_x = SCREEN_WIDTH / 2 / float(SCREEN_WIDTH);
  float size_y = SCREEN_HEIGHT / 2 / float(SCREEN_HEIGHT);

  std::shared_ptr<Image> image = get_test_image();
  TextureGl td(image);
  TextureDrawData image_data(v2(0,0), v2(size_x/2,size_y/2));

  FrameBufferGL fb(100, 100);
  TextureDrawData data(v2(0,0), v2(size_x,size_y));

  for(int i = 0; i < 200; i++){
    SDL_GL_SwapWindow(TestSuite::shared_resource_->window);
    glClearColor(0., 0.5, 0.5, 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    fb.SetClearColor(rgb(abs(cos(2*3.14*i/200.)),0,0));
    fb.Clear();

    fb.Target();
    image_data.angle = 360*i/100;
    image_data.pos = v2(0.1).Rotated(360*i/100);
    draw_textured_quad(td, image_data);
    fb.Untarget();

    glViewport( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
    data.angle = 360*i/100;
    draw_fb_quad(fb, data);

    SDL_Delay(20);
  }
}

TEST_F(TestSuite, shader_uniform) {
  GTEST_COUT << "Shader change color based on uniform" << std::endl;
  if(false){
    // legacy https://community.khronos.org/t/glbegin-glend-with-custom-shader-effect/73276/3
    const std::string vert = R"(
      void main(void)
      {
        gl_Position = ftransform();
        gl_TexCoord [0] = gl_MultiTexCoord0;
      }
    )";

    const std::string frag = R"(
        uniform sampler2D text_in;
        void main(void)
        {
          gl_FragColor = texture2D(text_in, gl_TexCoord[0].st);
        }
      )";
  }

  const std::string frag_uni = R"(
      #version 300 es
      precision mediump float;
      in vec2 v_texCoord;
      uniform float var0;
      uniform sampler2D text_0;
      out vec4 fragColor;
      void main(){
        fragColor = texture2D(text_0, v_texCoord);
        fragColor.x = var0;
        fragColor.y = max(v_texCoord.x, v_texCoord.y);
      }
    )";

  std::shared_ptr<Image> image = get_test_image();
  float size_x = SCREEN_WIDTH / 20 / float(SCREEN_WIDTH);
  float size_y = SCREEN_HEIGHT / 20 / float(SCREEN_HEIGHT);
  TextureGl td(image);

  ShaderGl shader;
  shader.LoadVert(vert);
  shader.LoadFrag(frag_uni);
  shader.CreateProgram();
  shader.AddUniform("text_0");
  shader.AddUniform("var0");

  int n_quads = 100;
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
    unsigned int id = qad.Add(data);
    ids.push_back(id);
    float speed = 1+rand()%10;
    directions.push_back(v2(speed/SCREEN_WIDTH,0).Rotated(rand()%360));
  }

  for(int i = 0; i < 200; i++){
    SDL_GL_SwapWindow(TestSuite::shared_resource_->window);
    glClearColor(0., 0.5, 0.5, 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    for(int qi = 0; qi < n_quads; qi ++){
      TextureDrawData* quad = quads[qi];
      quad->pos += directions[qi];
      if(quad->pos.x < -1) quad->pos.x = 1;
      if(quad->pos.x >  1) quad->pos.x = -1;
      if(quad->pos.y < -1) quad->pos.y = 1;
      if(quad->pos.y >  1) quad->pos.y = -1;
      quad->size = v2(size_x,size_y)*(0.75 + 0.5*cos(3.14 * (i / 200 + qi / n_quads)));
      qad.SetPos(ids[qi], quad);
      if(not (rand()%5)) directions[qi] = directions[qi].Rotated(rand()%10);
    }

    // draw quads
    td.Bind();
    shader.Bind();
    shader.EnableTexture(shader.GetUniform("text_0"), 0);
    float var0 = 0.5 + 0.5 * cos(3.14 * i / 200);
    shader.UpdateUniform1f(shader.GetUniform("var0"), var0);
    qad.Draw();
    shader.Unbind();
    td.Unbind();
    SDL_Delay(30);
  }
}

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

  int n_quads = 1000;
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
    unsigned int id = qad.Add(data);
    ids.push_back(id);
    float speed = 1+rand()%10;
    directions.push_back(v2(speed/SCREEN_WIDTH,0).Rotated(rand()%360));
  }

  for(int i = 0; i < 200; i++){
    SDL_GL_SwapWindow(TestSuite::shared_resource_->window);
    glClearColor(0., 0.5, 0.5, 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    for(int qi = 0; qi < n_quads; qi ++){
      TextureDrawData* quad = quads[qi];
      quad->pos += directions[qi];
      if(quad->pos.x < -1) quad->pos.x = 1;
      if(quad->pos.x >  1) quad->pos.x = -1;
      if(quad->pos.y < -1) quad->pos.y = 1;
      if(quad->pos.y >  1) quad->pos.y = -1;
      quad->size = v2(size_x,size_y)*(0.75 + 0.5*cos(3.14 * (i / 200 + qi / n_quads)));
      qad.SetPos(ids[qi], quad);

      if(not (rand()%10)) directions[qi] = directions[qi].Rotated(rand()%60-30);

      if(not (rand()%20)){
        float width = (rand() + 100) % 1000 / 1000.;
        if(width > 1.) width = 0.9;
        quad->tpos = v2(0,0);
        quad->tsize = v2(width, width);
        qad.SetText(ids[qi], quad);
      }
    }

    // draw quads
    td.Bind();
    shader.Bind();
    GLint position = glGetUniformLocation(shader.GetProgramId(), "text_0");
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(position, 0);
    qad.Draw();
    shader.Unbind();
    td.Unbind();
    SDL_Delay(30);
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
    GLint position = glGetUniformLocation(shader.GetProgramId(), "text_0");
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
  int & vlevel = msg_verbose_lvl();
  vlevel = verbose::VERBOSE;
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
