// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_CORE_HH
#define PMGDLIB_CORE_HH 1

#include "pmgdlib_defs.h"
#include "pmgdlib_msg.h"
#include "pmgdlib_math.h"
#include "pmgdlib_string.h"
#include "pmgdlib_graph.h"
#include "pmgdlib_storage.h"
#include <stack>

namespace pmgd {

  class Core {};

  // mouse base class
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

  // keyboard base class
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

  // io related items =====================================================================================================
  class IoTxt {
    public:
    virtual std::string Read(const std::string & path) {return "dummy data";};
    virtual int Write(const std::string & path, const std::string & data) {return PM_SUCCESS;};
    virtual ~IoTxt(){};
  };

  class Image;
  class IoImage {
    public:
    virtual std::shared_ptr<Image> Read(const std::string & path) {return nullptr;};
    virtual int Write(const std::string & path, std::shared_ptr<Image> image) {return PM_SUCCESS;};
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
  class Texture : public BaseMsg {
    /// base abstract texture class
    public:
    virtual void Bind()   = 0;
    virtual void Unbind() = 0;
    virtual ~Texture() {};
  };

  struct TexTile {
    /// TexTile store the info about position & size of part of image in Texture normalized to 1. space
    TexTile() {}
    TexTile(v2 tp, v2 ts) : tpos(tp), tsize(ts) {}
    v2 tpos, tsize;

    // TODO void Print(){ msg(__PFN__, pos, size, tpos, tsize); };
  };

  struct TextureDrawData {
    v2 pos = v2(0,0), size = v2(0.5,0.5);
    v2 tpos = v2(0,0), tsize = v2(1,1);
    float angle = 0;
    bool flip_x = false, flip_y = false;

    TextureDrawData(){}
    TextureDrawData(v2 pos, v2 size){
      this->pos = pos;
      this->size = size;
    }
  };

  class TextureAtlas {
    private:
      std::unordered_map<std::string, TexTile> atlas;
      v2 size;

    public:
      TexTile * GetTexTile(const std::string & key){
        auto ptr = atlas.find(key);
        if(ptr == atlas.end()) return nullptr;
        return &(ptr->second);
      }

      void AddTexTile(const std::string & key, const v2 & tp, const v2 & ts){
        if(GetTexTile(key)) return;
        atlas[key] = TexTile(tp, ts);
      }

      std::string GenItemKey(int x, int y) const {
        return "X" + std::to_string(x) + "Y" + std::to_string(y);
      };

      std::string GenItemKey(std::string name, int x, int y) const {
        return name + "_" + std::to_string(x) + "_" + std::to_string(y);
      };
  };

  class ShaderUniform1f {
    int index;
    std::string name;
    float val;
  };

  class Shader : public BaseMsg {
    public:
    virtual int LoadVert(const std::string & text) = 0;
    virtual int LoadFrag(const std::string & text) = 0;
    virtual int CreateProgram() = 0;
    virtual int AddUniform(const std::string name) = 0;
    virtual int GetUniform(const std::string name) = 0;
    virtual void UpdateUniform1f(const int & pos, const float & val) = 0;
    virtual void EnableTexture(const int & pos, const int index) = 0;
    virtual ~Shader(){};
  };

  class QuadsArray : public BaseMsg {
    virtual unsigned int IndexToId(unsigned int quad_index) = 0;
    virtual unsigned int IdToIndex(unsigned int id) = 0;
    virtual bool IsFreeIndex(unsigned int quad_index) = 0;
    virtual unsigned int GetDefaultId(){
      if(++last_quad_id >= max_quads_number) last_quad_id = 0;
      return last_quad_id;
    }

    protected:
    bool dirty = false;
    unsigned int last_quad_id = -1, max_quads_number = 0;
    std::stack<unsigned int> free_positions;
    virtual unsigned int FindFreePosition(){
      /// search free position in data array
      if(free_positions.size()) {
        unsigned int index = free_positions.top();
        free_positions.pop();
        return index;
      }
      for(unsigned int i = last_quad_id+1; i < max_quads_number; i++){
        if(IsFreeIndex(i)){ last_quad_id = i; return i; }
      }
      for(unsigned int i = 0; i <= last_quad_id; i++){
        if(IsFreeIndex(i)){ last_quad_id = i; return i; }
      }
      msg_warning("can't find free position");
      return GetDefaultId();
    }

    public:
    QuadsArray(unsigned int max_quads_number){ this->max_quads_number = max_quads_number; };
    virtual ~QuadsArray(){};

    //! add element to the array
    virtual unsigned int Add(TextureDrawData * quad_data) = 0;

    //! update array data full
    virtual void Set(const unsigned int & id, TextureDrawData * quad_data) = 0;

    //! update array data position only
    virtual void SetPos(const unsigned int & id, TextureDrawData * quad_data) = 0;

    //! update array data texture only
    virtual void SetText(const unsigned int & id, TextureDrawData * quad_data) = 0;

    //! change position of element by shift 2d value
    virtual void Move(const unsigned int & id, const v2 & shift) = 0;

    //! remove all elements from array
    virtual void Clean(){};

    //! remove one elements from array
    virtual void Remove(const unsigned int & id) = 0;

    //! draw
    virtual void Draw() = 0;
  };

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

    // aliases
    bool gl;

    void Finilize(){
      gl = accelerator == "GL";
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

  class Window {
    public:
    virtual ~Window() {}
  };

  class ScenePipeline : public BaseMsg {
    PipelineGraph<std::string> pg;
    std::vector<std::string*> pipeline;

    public:
    bool builded = false;

    int AddChain(const std::string & chain){
      builded = false;
      return add_strdata_to_pipeline(chain, pg);
    }

    int Build(){
      pipeline = pg.GetPipeline();
      msg_verbose(join_string_ptrs(pipeline));
      builded = true;
      return pipeline.size() ? PM_SUCCESS : PM_ERROR;
    }
  };

  class FrameDrawer : public BaseMsg {
  };

  class TextureDrawer : public BaseMsg {
    public:
    std::string shader_id, texture_id;
    TextureDrawData data;
    Texture *texture;
    Shader *shader;
    TextureDrawer(){};
    virtual void Draw(){}
    virtual ~TextureDrawer(){};
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
      std::vector<std::shared_ptr<ScenePipeline>> pipelines = Objects<ScenePipeline>();
      int ret_tot = PM_SUCCESS;
      for(auto pipeline : pipelines){
        int ret = pipeline->Build();
        if(ret != PM_SUCCESS){
          ret_tot = ret;
        }
      }
      return ret_tot;
    }

    int SetPipeline(std::string & key){
      std::shared_ptr<ScenePipeline> pipeline = Get<ScenePipeline>(key);

      if(pipeline == nullptr){
        msg_warning("Scene", quote(id), "does not have pipeline", quotec(key));
        return PM_ERROR_404;
      }
      active_pipeline_id = key;
      active_pipeline = Get<ScenePipeline>(key).get();
      return PM_SUCCESS;
    }
  };

<<<<<<< HEAD:lib/pmgdlib_core.h
  class SceneRender : public BaseMsg {
    public:
    virtual void Draw(std::shared_ptr<Scene> scene){
      /// Draw Cycle (unoptimized)
      /// 1. set target
      /// 2. bind source (shader, texture, buffers)
      /// 3. blit source
      /// 4. unbind source
      /// 5. unset target
    }
    virtual ~SceneRender(){};
  };

  class SysFactory {
=======
  class SysFactory : public BaseMsg {
>>>>>>> master:pmgdlib_core.h
    public:
    virtual std::shared_ptr<Window> CreateWindow(const SysOptions & opts) {return nullptr;}
    virtual ~SysFactory() {};
  };

  class AccelFactory {
    /// accelerated class object maker
    public:
    virtual int InitAccel(const SysOptions & opts){return PM_SUCCESS;}
    virtual std::shared_ptr<Texture> MakeTexture(std::shared_ptr<Image> img) {return nullptr;}
    virtual std::shared_ptr<Shader> MakeShader(const std::string & vert_txt, const std::string & frag_txt) {return nullptr;}
    virtual std::shared_ptr<FrameDrawer> MakeFrameDrawer(){return nullptr;}
    virtual std::shared_ptr<TextureDrawer> MakeTextureDrawer(){return nullptr;}
    virtual std::shared_ptr<SceneRender> MakeSceneRender(){return nullptr;}
    virtual ~AccelFactory() {};
  };
};

#endif
