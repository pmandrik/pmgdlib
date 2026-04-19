// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_CORE_HH
#define PMGDLIB_CORE_HH 1

#include "pmgdlib_defs.h"
#include "pmgdlib_msg.h"
#include "pmgdlib_math.h"
#include "pmgdlib_time.h"
#include "pmgdlib_string.h"
#include "pmgdlib_graph.h"
#include "pmgdlib_storage.h"
#include <stack>

namespace pmgd {
  // mouse base class =====================================================================================================
  class Mouse {
    public:
      void Tick(){
        left_up   = false;
        right_up  = false;
        middle_up = false;
        left_down   = false;
        right_down  = false;
        middle_down = false;
        wheel = 0;
      }

      int x, y, wheel;
      bool left_hold, right_hold, middle_hold;
      bool left_down, right_down, middle_down;
      bool left_up,   right_up,   middle_up;

      void SetWheel(float w){
        this->wheel = w;
      }

      void SetXY(float x, float y){
        this->x = x;
        this->y = y;
      }
  };

  // keyboard base class =====================================================================================================
  class Keyboard {
    public:
      Keyboard(){
        hor = 0; ver = 0;
        actz = false; actx = false; exit = false; screenshoot = false; hor_pressed = false; ver_pressed = false;
        for(int key = 0; key < 232; key++){
          keyboard_pressed[key] = 0;
          keyboard_holded[key]  = 0;
          keyboard_raised[key]  = 0;
        }
      }

      void Update(int key, bool pressed){
        /// Unsafe
        if(pressed){
          if(not keyboard_holded[key]){
            keyboard_pressed[key] = true;
            keyboard_holded[key] = true;
          }
          else keyboard_pressed[key] = false;
        } else {
          if(keyboard_holded[key]){
            keyboard_holded[key] = false;
            keyboard_raised[key] = true;
            keyboard_pressed[key] = false;
          }
          else keyboard_raised[key] = false;
        }
      }

      void Tick(){
        //! TODO move to logical keyboard from physical
        //! update keyboard in the implementation
        //! call this to update some regular keyboard events
        /*
        hor = check_sign(Holded(key::D) + Holded(key::RIGHT) - Holded(key::A) - Holded(key::LEFT));
        ver = check_sign(Holded(key::W) + Holded(key::UP) - Holded(key::S) - Holded(key::DOWN));
        actz = Pressed(key::Z) + Pressed(key::K) + Pressed(key::RETURN) + Pressed(key::SPACE);
        actx = Pressed(key::X) + Pressed(key::L);
        hor_pressed = check_sign(Pressed(key::D) + Pressed(key::RIGHT) - Pressed(key::A) - Pressed(key::LEFT));
        ver_pressed = check_sign(Pressed(key::W) + Pressed(key::UP) - Pressed(key::S) - Pressed(key::DOWN));
        exit = Pressed(key::ESCAPE);
        screenshoot = Holded(key::KP_0);
        */
      }

      inline bool Pressed(int key){ return keyboard_pressed[key]; }
      inline bool Holded(int key) { return keyboard_holded[key];  }
      inline bool Raised(int key) { return keyboard_raised[key];  }

      int hor, ver;
      bool actz, actx, exit, screenshoot, hor_pressed, ver_pressed;
      bool keyboard_pressed[232], keyboard_holded[232], keyboard_raised[232];
  };

  // core =====================================================================================================
  class Core {
    protected:
    Clocker* clocker;
    Mouse* mouse;
    Keyboard* keyboard;

    void TickCommon(){
      /// sleep a litle bit between frames
      clocker->Tick();
    }

    public:
    virtual void Tick() = 0;
    virtual ~Core(){}
  };

  // io related items =====================================================================================================
  class IoTxt {
    public:
    virtual std::string Read(const std::string & path) {return "dummy data";};
    virtual int Write(const std::string & path, const std::string & data) {return PM_SUCCESS;};
    virtual ~IoTxt(){};
  };

  class Image;
  class IoImage : public BaseMsg {
    public:
    virtual std::shared_ptr<Image> Read(const std::string & path) {
      msg_warning("function not implemented");
      return nullptr;
    };
    virtual int Write(const std::string & path, std::shared_ptr<Image> image) {
      msg_warning("function not implemented");
      return PM_SUCCESS;
    };
    virtual ~IoImage(){};
  };

  class IO {
    public:
    std::shared_ptr<IoTxt> txt_imp = std::make_shared<IoTxt>();
    std::shared_ptr<IoImage> img_imp = std::make_shared<IoImage>();

    std::string ReadTxt(const std::string & path){
      return txt_imp->Read(path);
    }

    std::shared_ptr<Image> ReadImage(const std::string & path){
      return img_imp->Read(path);
    }

    int WriteImage(const std::string & path, std::shared_ptr<Image> image){
      return img_imp->Write(path, image);
    }
  };

  // drawing related items =================================================================================================
  class FrameBuffer : public BaseMsg {
    /// frame buffer is a texture + depth buffer stored at GPU and with fast GPU access
    protected:
    rgb clear_color = rgb(0, 255, 255);
    int size_x, size_y;

    public:
    FrameBuffer(int size_x, int size_y){
      this->size_x = size_x;
      this->size_y = size_y;
    };

    void SetClearColor(rgb color){ clear_color = color; }

    virtual ~FrameBuffer(){};

    //! set this buffer as target
    virtual void Target() = 0;

    //! unset this buffer as target
    virtual void Untarget() = 0;

    //! fill buffer with some 0 data
    virtual void Clear() = 0;

    //! same as Clear
    void Clean(){ Clear(); };

    //! bind frame buffer texture, so can be used to draw
    virtual void BindTexture() = 0;

    //! unbind frame buffer texture
    virtual void UnbindTexture() = 0;

    v2 GetSize() const { return v2(size_x, size_y); };
  };

  class DoubleFrameBuffer : public BaseMsg {
    public:
    std::shared_ptr<FrameBuffer> active, back;

    public :
    DoubleFrameBuffer(std::shared_ptr<FrameBuffer> fb1, std::shared_ptr<FrameBuffer> fb2){
      active = fb1;
      back   = fb2;
    }

    void Target(){ active->Target(); }
    void Untarget(){ active->Untarget(); }
    void Clear(){ active->Clear(); }
    void BindTexture(){ active->BindTexture(); }
    void UnbindTexture(){ active->BindTexture(); }

    void BindBackTexture(){ back->BindTexture(); }
    void UnbindBackTexture(){ back->BindTexture(); }
    void Flip(){ swap(active, back); }

    v2 GetSize() const {return active->GetSize();};
  };

  struct SysOptions {
    // core options
    int screen_width = 640;
    int screen_height = 480;
    std::string multimedia_library;
    std::string accelerator;
    std::string io;
    std::string img;
    int fps = 60;

    // aliases
    bool gl;
    bool soft_render;

    void Finilize(){
      gl = accelerator == "GL";
      soft_render = accelerator == "";
    }

    std::string AsString(){
      std::string answer;
      std::string tabs2 = std::string(2, ' ');
      std::string tabs4 = std::string(4, ' ');
      answer += "SysOptions(\n";
      answer += tabs2 + "Backends:\n";
      answer += tabs4 + "io backend = " + io + "\n";
      answer += tabs4 + "img backend = " + img + "\n";
      answer += tabs4 + "multimedia_library backend = " + multimedia_library + "\n";
      answer += tabs4 + "accelerator backend = " + accelerator + "\n";
      answer += tabs2 + "Screen options:" + "\n";
      answer += tabs4 + "width, height = " + std::to_string(screen_width) + " " + std::to_string(screen_height) + "\n";
      answer += ")";
      return answer;
    }
  };

  class ScenePipeline : public BaseMsg {
    std::shared_ptr<PipelineGraph<std::string>> pg;

    public:
    int AddChain(const std::string & chain){
      return 0;
      // return add_strdata_to_pipeline(chain, pg);
    }
  };

  class Scene : public DataContainer {
    std::string active_pipeline_id;
    ScenePipeline * active_pipeline = nullptr;

    public:
    std::string id;
    Scene(const std::string & id){
      this->id = id;
    }
    virtual ~Scene(){};

    int Warm(){
      /// Warm up pipelines
      return PM_SUCCESS;
    }

    int SetPipeline(std::string & key){
      std::shared_ptr<ScenePipeline> pipeline = Get<ScenePipeline>(key);

      if(pipeline == nullptr){
        msg_warning("Scene", quote(id), "does not have pipeline", quotec(key));
        return PM_ERROR_404;
      }
      active_pipeline_id = key;
      active_pipeline = pipeline.get();
      return PM_SUCCESS;
    }

    ScenePipeline * GetPipeline(){ return active_pipeline; }
  };

  class Window {
    public:
    virtual ~Window() {}
  };
  class Render;
  class SysFactory : public BaseMsg {
    public:
    virtual std::shared_ptr<Window> MakeWindow(const SysOptions & opts) {return nullptr;}
    virtual std::shared_ptr<Render> MakeRender(const SysOptions & opts, std::shared_ptr<Window> w) {return nullptr;}
    virtual std::shared_ptr<Core> MakeCore(const SysOptions & opts) {return nullptr;}
    virtual ~SysFactory() {};
  };

  class Texture;
  class Shader;
  class FrameDrawer;
  class TextureDrawer;
  class SimpleDrawer;
  class AccelFactory {
    /// accelerated class object maker
    public:
    virtual int InitAccel(const SysOptions & opts){return PM_SUCCESS;}
    virtual std::shared_ptr<Texture> MakeTexture(std::shared_ptr<Image> img) {return nullptr;}
    virtual std::shared_ptr<Shader> MakeShader(const std::string & vert_txt, const std::string & frag_txt) {return nullptr;}
    virtual std::shared_ptr<FrameDrawer> MakeFrameDrawer(){return nullptr;}
    virtual std::shared_ptr<TextureDrawer> MakeTextureDrawer(){return nullptr;}
    virtual std::shared_ptr<SimpleDrawer> MakeSimpleDrawer(){return nullptr;}
    // virtual std::shared_ptr<SceneRender> MakeSceneRender(){return nullptr;}
    virtual ~AccelFactory() {};
  };
};

#endif
