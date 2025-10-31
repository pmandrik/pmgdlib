#include <test_common.h>

#define USE_GL 1
#define USE_SDL 1
#define USE_STB 1

#include <SDL.h>
#include <SDL_opengl.h>
#include <pmgdlib_gl.h>
#include <pmgdlib_factory.h>
#include <vector>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

//=======================================================================================
class TestContext {
  public:
  std::shared_ptr<Window> window;

  TestContext(){
  }

  ~TestContext(){
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
//=======================================================================================
TEST_F(TestSuite, factory_sys) {
  const string cfg_raw = R"(
    <!-- system -->
    <sys screen_width="800" screen_height="600"/> 
    <sys io_backend="SDL" img_backend="STB"/> 
    <sys multimedia_library="SDL" accelerator="GL"/>

    <!-- textures -->
    <texture id="t1" image_path="../data/img.png"/>
    <texture id="t2" image_id="img" image_path="../data/img.png"/>

    <!-- shaders -->
    <shader id="s1" vert="../data/shaders/default/def.vert" frag="../data/shaders/default/def.frag"/>

    <!-- scene --> 
    <scene id="scene">
      <texture_drawer id="td1" texture="t1" once="1"/>
      <texture_drawer id="td2" texture="t2" shader="s1"/>

      <frame_drawer id="fd1" shader="s1"/>

      <pipeline id="default">
        <chain data="td1->fd1"/>
        <chain data="td2->fd1->screen"/>
      </pipeline>
    </scene>
  )";

  ConfigLoader cfg_loader;
  Config cfg = cfg_loader.LoadXmlCfg(cfg_raw);
  SysOptions options = cfg_loader.GetSysOptions(cfg);
  Backend backend = get_backend(options);

  shared_resource_->window = backend.factory->CreateWindow(options);
  SceneDataLoader loader(backend);
  EXPECT_TRUE(loader.Load(cfg) == PM_SUCCESS);
  EXPECT_TRUE(loader.dc.Get<Texture>("t1") != nullptr);
  EXPECT_TRUE(loader.dc.Get<Texture>("t2") != nullptr);
  EXPECT_TRUE(loader.dc.Get<Shader>("s1") != nullptr);
  EXPECT_TRUE(loader.dc.Get<Scene>("scene") != nullptr);

  auto scene = loader.dc.Get<Scene>("scene");
  auto td1 = scene->Get<TextureDrawer>("td1");
  auto td2 = scene->Get<TextureDrawer>("td2");
  auto fd1 = scene->Get<FrameDrawer>("fd1");
  EXPECT_TRUE(td1 != nullptr);
  EXPECT_TRUE(td2 != nullptr);
  EXPECT_TRUE(fd1 != nullptr);

  EXPECT_TRUE(scene->Warm() == PM_SUCCESS);

  SDL_Delay(1000);
}

//=======================================================================================
int main(int argc, char **argv) {
  int & vlevel = msg_verbose_lvl();
  vlevel = verbose::VERBOSE;
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}