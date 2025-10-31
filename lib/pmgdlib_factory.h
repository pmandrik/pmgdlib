// P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib
#ifndef PMGDLIB_FACTORY_HH
#define PMGDLIB_FACTORY_HH 1

#include "pmgdlib_string.h"
#include "pmgdlib_core.h"
#include "pmgdlib_config.h"
#include "pmgdlib_image.h"

#ifdef USE_SDL
  #include "pmgdlib_sdl.h"
#endif

#ifdef USE_GL
  #include "pmgdlib_gl.h"
#endif

namespace pmgd {

  class Factory {
    public:
    std::shared_ptr<AccelFactory> accel_imp = std::make_shared<AccelFactory>();
    std::shared_ptr<SysFactory> sys_imp = std::make_shared<SysFactory>();

    int InitAccel(const SysOptions & opts){ return accel_imp->InitAccel(opts);}
    
    template<typename... Args> std::shared_ptr<Texture> 
    MakeTexture(Args... args){ return accel_imp->MakeTexture(std::forward<Args>(args)...); }

    template<typename... Args> std::shared_ptr<Shader>
    MakeShader(Args... args){return accel_imp->MakeShader(std::forward<Args>(args)...);}

    std::shared_ptr<Window> CreateWindow(const SysOptions & opts){return sys_imp->CreateWindow(opts);}

    template<typename... Args> std::shared_ptr<Scene>
    MakeScene(Args... args){ return std::make_shared<Scene>(std::forward<Args>(args)...); }

    template<typename... Args> std::shared_ptr<ScenePipeline>
    MakeScenePipeline(Args... args){ return std::make_shared<ScenePipeline>(std::forward<Args>(args)...); }

    template<typename... Args> std::shared_ptr<FrameDrawer> 
    MakeFrameDrawer(Args... args){return accel_imp->MakeFrameDrawer(std::forward<Args>(args)...);}

    template<typename... Args> std::shared_ptr<TextureDrawer>
    MakeTextureDrawer(Args... args){return accel_imp->MakeTextureDrawer(std::forward<Args>(args)...);}

    template<typename... Args> std::shared_ptr<SceneRender>
    MakeSceneRender(Args... args){return accel_imp->MakeSceneRender(std::forward<Args>(args)...);}
  };

  class Backend {
    public:
    std::shared_ptr<IO> io = std::make_shared<IO>();
    std::shared_ptr<Factory> factory = std::make_shared<Factory>();
  };

  Backend get_backend(const SysOptions & options){
    int verbose_lvl = msg_verbose_lvl();

    msg_debug("Factory backend start ...");
    Backend back;

    #ifdef USE_GL
    if(options.accelerator == "GL"){
      msg_debug("use GL accelerator backend");
      back.factory->accel_imp = std::make_shared<AccelFactoryGL>();
    }
    #endif

    #ifdef USE_SDL
    if(options.io == "SDL"){
      msg_debug("use SDL IO backend");
      back.io->txt_imp = std::make_shared<IoTxtSDL>();
    }

    if(options.multimedia_library == "SDL"){
      msg_debug("use SDL multimedia backend");
      back.factory->sys_imp = std::make_shared<SysFactorySDL>();
    }
    #endif

    #ifdef USE_STB
    if(options.img == "STB"){
      msg_debug("use STB image IO backend");
      back.io->img_imp = std::make_shared<IoImageStb>();
    }
    #endif

    msg_debug("Factory backend ok ...");
    return back;
  };

  class SceneDataLoader : public BaseMsg {
    // process stack
    std::vector<const ConfigItem*> processed_stack;
    std::string GetFatherAttribute(std::string attr, unsigned int depth=1){
      if(depth > processed_stack.size()) return "";
      const ConfigItem * father = processed_stack.at(processed_stack.size() - depth);
      return father->Attribute(attr);
    }

    // loaders
    int LoadImage(std::string id, std::string path){
      msg_debug("load image, id =", quotec(id), "path =", quote(path));
  
      if(not path.size()){
        msg_warning("empty path");
        return PM_ERROR_INCORRECT_ARGUMENTS;
      }
  
      if(not id.size()){
        msg_debug("id = '', use full file name as id");
        id = path;
      }
  
      std::shared_ptr<Image> image = io->ReadImage(path);
      if(not image){
        msg_warning("failed to load image");
        return PM_ERROR_IO;
      }

      dc.Add(id, image);
      return PM_SUCCESS;
    }

    int LoadTexture(const ConfigItem & cfg){
      // id = "id"
      // image = "image_id", "image_path"
      std::string id = cfg.Attribute("id");

      /// find/load image
      std::string image_id = cfg.Attribute("image_id");
      std::string image_path = cfg.Attribute("image_path");
      if(image_path.size()){
        msg_debug("texture image is passed by path", quote(image_path));
        int err = LoadImage(image_id, image_path);
        if(err != PM_SUCCESS){
          msg_warning("texture image load failed", quote(image_path));
          return err;
        }

        if(not image_id.size()) image_id = image_path;
      } else msg_debug("texture image is passed by id =", quote(image_id));
      

      /// build texture
      std::shared_ptr<Image> img = dc.Get<Image>(image_id);
      if(img == nullptr){
        msg_warning("can't find image", quote(image_id));
        return PM_ERROR_INCORRECT_ARGUMENTS;
      }
      std::shared_ptr<Texture> texture = factory->MakeTexture(img);
      dc.Add(id, texture);
      return PM_SUCCESS;
    }

    int LoadShader(const ConfigItem & cfg){
      std::string id = cfg.Attribute("id");
      std::string vert_path = cfg.Attribute("vert");
      std::string frag_path = cfg.Attribute("frag");

      std::string vert_txt = io->ReadTxt(vert_path);
      std::string frag_txt = io->ReadTxt(frag_path);

      std::shared_ptr<Shader> shader = factory->MakeShader(vert_txt, frag_txt);
      dc.Add(id, shader);
      return PM_SUCCESS;
    }

    int LoadScene(const ConfigItem & cfg){
      std::string id = cfg.Attribute("id");
      std::shared_ptr<Scene> scene = factory->MakeScene(id);
      dc.Add(id, scene);
      return PM_SUCCESS;
    }

    int LoadPipelineChain(const ConfigItem & cfg){
      std::shared_ptr<Scene> scene = dc.Get<Scene>(GetFatherAttribute("id", 2));
      std::string pipeline_id = GetFatherAttribute("id");
      
      if(CWM(not scene or not pipeline_id.size(), "pipeline", quotec(pipeline_id), "required to be a part of scene and pipeline")) 
        return PM_ERROR_SCHEMA;

      auto pipeline = scene->Get<ScenePipeline>(pipeline_id);
      if(pipeline == nullptr) {
        scene->Add(pipeline_id, factory->MakeScenePipeline());
        pipeline = scene->Get<ScenePipeline>(pipeline_id);
      }

      std::string value = cfg.Attribute("data");
      return pipeline->AddChain(value);
    }

    int LoadFrameDrawer(const ConfigItem & cfg){
      std::shared_ptr<Scene> scene = dc.Get<Scene>(GetFatherAttribute("id"));
      if(CWM(not scene, "FrameDrawer is required to be a part of scene")) return PM_ERROR_SCHEMA;

      std::string id = cfg.Attribute("id");
      return scene->Add(id, factory->MakeFrameDrawer()); // TODO
    }

    int LoadTextureDrawer(const ConfigItem & cfg){
      std::shared_ptr<Scene> scene = dc.Get<Scene>(GetFatherAttribute("id"));
      if(CWM(not scene, "TextureDrawer is required to be a part of scene")) return PM_ERROR_SCHEMA;

      // TODO read pos, size
      // TODO read options

      std::shared_ptr<TextureDrawer> td = factory->MakeTextureDrawer();
      td->shader_id = cfg.Attribute("shader");
      td->texture_id = cfg.Attribute("texture");
      std::string id = cfg.Attribute("id");
      return scene->Add(id, td);
    }

    void AddRuleToCfg(Config & cfg, std::string key, std::vector <std::string> mandatory, std::vector <std::string> fathers,
      std::function<int(const ConfigItem&)> proccessor){
      ConfigSchema schema(mandatory, fathers);
      cfg.AddProcessingRule(key, schema, proccessor);
    }

    std::shared_ptr<IO> io;
    std::shared_ptr<Factory> factory;

    int FinilizeFrameDrawer(std::shared_ptr<FrameDrawer> obj){
      // obj->shader = dc.Get<Shader>(obj->shader_id);
      return PM_SUCCESS;
    }

    int FinilizeTextureDrawer(std::shared_ptr<TextureDrawer> obj){
      obj->shader = dc.Get<Shader>(obj->shader_id).get();
      obj->texture = dc.Get<Texture>(obj->texture_id).get();
      return PM_SUCCESS;
    }

    int FinilizeScene(std::shared_ptr<Scene> obj){
      return PM_SUCCESS;
    }

    template<typename T>
    int Finilize(DataContainer &dc, const std::string & id, std::function<int(std::shared_ptr<T>)> proccessor){
      std::vector<std::string> ids = dc.Ids<T>();
      int ret_tot = PM_SUCCESS;
      for(auto id : ids){
        msg_verbose("object", quote(id), "finalization");
        std::shared_ptr<T> obj = dc.Get<T>(id);
        int ret = proccessor(obj);
        if(ret != PM_SUCCESS) ret_tot = ret;
      }
      return ret_tot;
    }

    public:
    DataContainer dc;
    SceneDataLoader(const Backend & backend){
      io = backend.io;
      factory = backend.factory;
    }

    int Load(Config & cfg){
      msg_debug("start ...");
      msg_debug("cfg processing ...");
      AddRuleToCfg(cfg, "texture", {"id"}, {}, [this](const ConfigItem & c) {return this->LoadTexture(c);});
      AddRuleToCfg(cfg, "shader", {"id"}, {}, [this](const ConfigItem & c) {return this->LoadShader(c);});
      AddRuleToCfg(cfg, "scene", {"id"}, {}, [this](const ConfigItem & c) {return this->LoadScene(c);});
      AddRuleToCfg(cfg, "chain", {"data"}, {"pipeline", "scene"}, [this](const ConfigItem & c) {return this->LoadPipelineChain(c);});
      AddRuleToCfg(cfg, "frame_drawer", {"id"}, {}, [this](const ConfigItem & c) {return this->LoadFrameDrawer(c);});
      AddRuleToCfg(cfg, "texture_drawer", {"id"}, {}, [this](const ConfigItem & c) {return this->LoadTextureDrawer(c);});
      processed_stack.clear();
      int ret = cfg.ProcessItems(processed_stack);
      if(ret != PM_SUCCESS) return ret;

      //! finalize
      msg_debug("objects finalization ...");
      Finilize<Scene>(dc, "scene", [this](std::shared_ptr<Scene> c) {
        Finilize<FrameDrawer>(*c, "frame_drawer", [this](std::shared_ptr<FrameDrawer> c) {return this->FinilizeFrameDrawer(c);});
        Finilize<TextureDrawer>(*c, "texture_drawer", [this](std::shared_ptr<TextureDrawer> c) {return this->FinilizeTextureDrawer(c);});
        return this->FinilizeScene(c);
      });

      return PM_SUCCESS;
    }
  };


};

#endif
