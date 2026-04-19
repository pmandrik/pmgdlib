// P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib
#ifndef PMGDLIB_FACTORY_HH
#define PMGDLIB_FACTORY_HH 1

#include "pmgdlib_string.h"
#include "pmgdlib_core.h"
#include "pmgdlib_core_render.h"
#include "pmgdlib_config.h"
#include "pmgdlib_image.h"

#ifdef USE_SDL
  #include "pmgdlib_sdl.h"
#endif

#ifdef USE_GL
  #include "pmgdlib_gl.h"
#endif

namespace pmgd {
  // ======= Factory ====================================================================
  class Factory {
    public:
    std::shared_ptr<AccelFactory> accel_imp = std::make_shared<AccelFactory>();
    std::shared_ptr<SysFactory> sys_imp = std::make_shared<SysFactory>();

    int InitAccel(const SysOptions & opts){ return accel_imp->InitAccel(opts);}
    
    template<typename... Args> std::shared_ptr<Texture> 
    MakeTexture(Args... args){ return accel_imp->MakeTexture(std::forward<Args>(args)...); }

    template<typename... Args> std::shared_ptr<Shader>
    MakeShader(Args... args){return accel_imp->MakeShader(std::forward<Args>(args)...);}

    std::shared_ptr<Window> MakeWindow(const SysOptions & opts){return sys_imp->MakeWindow(opts);}
    template<typename... Args> std::shared_ptr<Render> 
    MakeRender(Args... args){return sys_imp->MakeRender(std::forward<Args>(args)...);}
    template<typename... Args> std::shared_ptr<Core>
    MakeCore(Args... args){return sys_imp->MakeCore(std::forward<Args>(args)...);}

    template<typename... Args> std::shared_ptr<Scene>
    MakeScene(Args... args){ return std::make_shared<Scene>(std::forward<Args>(args)...);}

    template<typename... Args> std::shared_ptr<ScenePipeline>
    MakeScenePipeline(Args... args){ return std::make_shared<ScenePipeline>(std::forward<Args>(args)...); }

    template<typename... Args> std::shared_ptr<FrameDrawer> 
    MakeFrameDrawer(Args... args){return accel_imp->MakeFrameDrawer(std::forward<Args>(args)...);}

    template<typename... Args> std::shared_ptr<TextureDrawer>
    MakeTextureDrawer(Args... args){return accel_imp->MakeTextureDrawer(std::forward<Args>(args)...);}

    template<typename... Args> std::shared_ptr<SimpleDrawer>
    MakeSimpleDrawer(Args... args){return accel_imp->MakeSimpleDrawer(std::forward<Args>(args)...);}

    //template<typename... Args> std::shared_ptr<SceneRender>
    //MakeSceneRender(Args... args){return accel_imp->MakeSceneRender(std::forward<Args>(args)...);}
  };

  // ======= Backend ====================================================================
  //! Backend combines Factory to create objects and IO to read data from storage
  class Backend : public IO, public Factory {};

  //! function to create and setup Backend class 
  std::shared_ptr<Backend> get_backend(const SysOptions & options);

  class Builder : public BaseMsg {
    std::shared_ptr<DataContainer> dc = nullptr;

    public:
    void SetDataSource(std::shared_ptr<DataContainer> dc_){ dc = dc_; }

    auto BuildGraph(std::string chain){
      auto pg = std::make_shared<PipelineGraph<std::string>>();
      int ret = add_strdata_to_pipeline(chain, *pg);
      if(ret != PM_SUCCESS){
        msg_error("failed to build pipeline fromn chain", quote(chain));
      }
      return pg;
    }

    auto BuildRenderPipeline(std::shared_ptr<PipelineGraph<std::string>> gr){
      auto p = std::make_shared<RenderPipeline>();
      if(dc == nullptr){
        msg_error("DataContainer is nullptr");
        return p;
      }
      auto parts = gr->GetPipelineGroupSource();
      for(auto part : parts){
        std::string source_id = part.source;
        std::string target_id = part.target;
        auto source = dc->Get<Drawable>(source_id);
        auto target = dc->Get<Drawable>(target_id);
      
        if(source == nullptr){
          msg_error("DataContainer can't find", quotec(source_id), "skip node");
          continue;
        }
        if(target == nullptr){
          msg_error("DataContainer can't find", quotec(target_id), "skip node");
          continue;
        }

        RenderPipelineItem pi;
        pi.source = source.get();
        pi.target = target.get();
        p->AddItem(pi);
      }
      return p;
    }
  };

  //! Create objects from ProtoObjects and put into DataContainer
  class ProtoBuilder : public BaseMsg {
    std::shared_ptr<Backend> back;
    std::shared_ptr<NdMap<ProtoObject>> ndmap = nullptr;
    std::vector<NdKey> namespaces;
    std::map<std::string, std::function<std::shared_ptr<void>(const ConfigItem*)>> processors;

    int BuildTexture(std::shared_ptr<ProtoObject> po){
      const ConfigItem * cfg = po->cfg_item;

      return PM_SUCCESS;
    }

    auto BuildShader(const ConfigItem* cfg){
      std::string vert_path = cfg->Attribute("vert");
      std::string frag_path = cfg->Attribute("frag");

      std::string vert_txt = back->ReadTxt(vert_path);
      std::string frag_txt = back->ReadTxt(frag_path);

      std::shared_ptr<Shader> obj = back->MakeShader(vert_txt, frag_txt);
      return obj;
    }

    auto BuildScene(const ConfigItem* cfg){
      std::string id = cfg->Attribute("id");
      std::shared_ptr<Scene> obj = back->MakeScene(id);
      return obj;
    }

    template<typename T>
    std::shared_ptr<T> GetDependence(const ConfigItem* cfg, std::string id_key, std::string type = ""){
      std::string id = cfg->Attribute(id_key);
      if(not type.size()) type = id_key;

      NdKey key(type, id);
      std::shared_ptr<ProtoObject> po = ndmap->GetOne(namespaces, key);
      if(po == nullptr) return nullptr;
      if(not po->IsWarm()) BuildObject(po);
      return std::static_pointer_cast<T>(po->object);
    }

    auto BuildSimpleDrawer(const ConfigItem* cfg){
      std::string texture_id = cfg->Attribute("texture");

      auto texture = GetDependence<Texture>(cfg, "texture");

      std::shared_ptr<SimpleDrawer> obj = back->MakeSimpleDrawer();
      return obj;
    }

    auto BuildPipeline(const ConfigItem* cfg){
      std::vector<std::string> chains = cfg->GetAttrsFromNested("chain", "data");
      // std::shared_ptr<Pipeline> obj = back->MakePipeline();
    }

    int BuildObject(std::shared_ptr<ProtoObject> po){
      //! do not reload object if warm
      if(po->IsWarm()){
        return PM_SUCCESS;
      }

      //! get config from proto object and build
      const ConfigItem* cfg = po->cfg_item;
      std::string type = cfg->type;

      auto find = processors.find(type);
      if(find == processors.end()){
        return PM_ERROR;
      }

      msg_info("start build", type);
      po->object = std::static_pointer_cast<void>(find->second(cfg));
      return PM_SUCCESS;
    }

    public:
    int BuildObjects(std::vector<NdKey> namespaces_, std::vector<std::shared_ptr<ProtoObject>> objects){
      namespaces = namespaces_;

      msg_debug("build start ...");
      int ret = PM_SUCCESS;
      for(auto object : objects){
        int retloc = BuildObject(object);
        if(retloc != PM_SUCCESS) ret = retloc;
      }
      msg_debug("build done ...");
      return ret;
    } 

    ProtoBuilder(std::shared_ptr<Backend> back_, std::shared_ptr<NdMap<ProtoObject>> ndmap_){
      back = back_;
      ndmap = ndmap_;

      processors["shader"] = [this](const ConfigItem* c) {return std::static_pointer_cast<void>(this->BuildShader(c));};
      processors["drawer"] = [this](const ConfigItem* c) {return std::static_pointer_cast<void>(this->BuildSimpleDrawer(c));}; 
      processors["scene"] = [this](const ConfigItem* c) {return std::static_pointer_cast<void>(this->BuildPipeline(c));};
    }
  };
};

#endif


/*


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
  
      std::shared_ptr<Image> image = back->ReadImage(path);
      if(not image){
        msg_warning("failed to load image");
        return PM_ERROR_IO;
      }

      dc->Add(id, image);
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
      std::shared_ptr<Image> img = dc->Get<Image>(image_id);
      if(img == nullptr){
        msg_warning("can't find image", quote(image_id));
        return PM_ERROR_INCORRECT_ARGUMENTS;
      }
      std::shared_ptr<Texture> texture = back->MakeTexture(img);
      dc->Add(id, texture);
      return PM_SUCCESS;
    }

    int LoadShader(const ConfigItem & cfg){
      std::string id = cfg.Attribute("id");
      std::string vert_path = cfg.Attribute("vert");
      std::string frag_path = cfg.Attribute("frag");

      std::string vert_txt = back->ReadTxt(vert_path);
      std::string frag_txt = back->ReadTxt(frag_path);

      std::shared_ptr<Shader> shader = back->MakeShader(vert_txt, frag_txt);
      dc->Add(id, shader);
      return PM_SUCCESS;
    }

    int LoadScene(const ConfigItem & cfg){
      std::string id = cfg.Attribute("id");
      std::shared_ptr<Scene> scene = back->MakeScene(id);
      dc->Add(id, scene);
      return PM_SUCCESS;
    }

    int LoadPipelineChain(const ConfigItem & cfg){
      std::shared_ptr<Scene> scene = dc->Get<Scene>(GetFatherAttribute("id", 2));
      std::string pipeline_id = GetFatherAttribute("id");
      
      if(CWM(not scene or not pipeline_id.size(), "pipeline", quotec(pipeline_id), "required to be a part of scene and pipeline")) 
        return PM_ERROR_SCHEMA;

      auto pipeline = scene->Get<ScenePipeline>(pipeline_id);
      if(pipeline == nullptr) {
        scene->Add(pipeline_id, back->MakeScenePipeline());
        pipeline = scene->Get<ScenePipeline>(pipeline_id);
      }

      std::string value = cfg.Attribute("data");
      return pipeline->AddChain(value);
    }

    int LoadFrameDrawer(const ConfigItem & cfg){
      std::shared_ptr<Scene> scene = dc->Get<Scene>(GetFatherAttribute("id"));
      if(CWM(not scene, "FrameDrawer is required to be a part of scene")) return PM_ERROR_SCHEMA;

      std::string id = cfg.Attribute("id");
      return scene->Add(id, back->MakeFrameDrawer()); // TODO
    }

    int LoadTextureDrawer(const ConfigItem & cfg){
      std::shared_ptr<Scene> scene = dc->Get<Scene>(GetFatherAttribute("id"));
      if(CWM(not scene, "TextureDrawer is required to be a part of scene")) return PM_ERROR_SCHEMA;

      // TODO read pos, size
      // TODO read options

      std::shared_ptr<TextureDrawer> td = back->MakeTextureDrawer();
      td->shader_id = cfg.Attribute("shader");
      td->texture_id = cfg.Attribute("texture");
      std::string id = cfg.Attribute("id");
      return scene->Add(id, td);
    }

    int LoadSimpleDrawer(const ConfigItem & cfg){
      std::shared_ptr<Scene> scene = dc->Get<Scene>(GetFatherAttribute("id"));
      if(CWM(not scene, "Drawer is required to be a part of scene")) return PM_ERROR_SCHEMA;

      std::shared_ptr<SimpleDrawer> d = back->MakeSimpleDrawer();
      std::string id = cfg.Attribute("id");
      return scene->Add(id, d);
    }

    std::string GetFatherAttribute(std::string attr, unsigned int depth=1){
      if(depth > processed_stack.size()) return "";
      const ConfigItem * father = processed_stack.at(processed_stack.size() - depth);
      return father->Attribute(attr);
    }

*/