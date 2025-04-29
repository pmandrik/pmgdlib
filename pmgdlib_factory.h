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
    std::shared_ptr<Texture> MakeTexture(std::shared_ptr<Image> img){ return accel_imp->MakeTexture(img); }
    std::shared_ptr<Shader> MakeShader(const std::string & vert_txt, const std::string & frag_txt){
      return accel_imp->MakeShader(vert_txt, frag_txt);
    }

    std::shared_ptr<Window> CreateWindow(const SysOptions & opts){return sys_imp->CreateWindow(opts);}
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

  template<typename T> std::string add_type_prefix(const std::string & name){return "default:" + name;}
  template<> std::string add_type_prefix<Image>(const std::string & name){return "image:" + name;}
  template<> std::string add_type_prefix<Shader>(const std::string & name){return "shader:" + name;}

  class DataContainer : public BaseMsg {
    private:
    std::unordered_map<std::string, std::shared_ptr<void>> data;

    public:
    template<typename T>
    std::shared_ptr<T> Get(const std::string & name){
      if( not name.size() ) return nullptr;
      const std::string id = add_type_prefix<T>(name);
      std::unordered_map<std::string, std::shared_ptr<void>>::const_iterator ptr = data.find(id);
      if(ptr == data.end()) {
        msg_warning("can't find", quote(name), ", return nullptr");
        return nullptr;
      }
      return std::static_pointer_cast<T>(ptr->second);
    }

    template<typename T>
    int Add(const std::string & name, std::shared_ptr<T> obj){
      const std::string id = add_type_prefix<T>(name);
      std::unordered_map<std::string, std::shared_ptr<void>>::const_iterator ptr = data.find(id);
      if(ptr != data.end()) {
        msg_warning("can't add duplicate", quote(name));
        return PM_ERROR_DUPLICATE;
      }
      msg_debug("add", quote(name));
      data[id] = std::static_pointer_cast<void>(obj);
      return PM_SUCCESS;
    }
  };

  class SceneDataLoader : public BaseMsg {
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
  
      // if(CheckId(images_container_id, id) != PM_SUCCESS){
      //   msg_warning("duplicate id");
      //   return PM_ERROR_DUPLICATE;
      // }
  
      std::shared_ptr<Image> image = io->ReadImage(path);
      if(not image){
        msg_warning("failed to load image");
        return PM_ERROR_IO;
      }

      dc.Add(id, image);
      return PM_SUCCESS;
    }

    int LoadTexture(const ConfigItem & cfg){
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
      


      return PM_SUCCESS;
    }

    std::shared_ptr<IO> io;
    std::shared_ptr<Factory> factory;
    DataContainer dc;

    public:
    SceneDataLoader(const Backend & backend){
      io = backend.io;
      factory = backend.factory;
    }

    std::vector<const ConfigItem*> processed_stack;
    int Load(Config & cfg){
      ConfigSchema texture_schema({"id"});
      ConfigProccessor load_texture = [this](const ConfigItem & c) { return this->LoadTexture(c); };
      cfg.AddProcessingRule("texture", texture_schema, load_texture);

      ConfigSchema shader_schema({"id"});
      ConfigProccessor load_shader = [this](const ConfigItem & c) { return this->LoadShader(c); };
      cfg.AddProcessingRule("shader", shader_schema, load_shader);

      ConfigSchema scene_schema({"id"});
      ConfigProccessor load_scene = [this](const ConfigItem & c) { return this->LoadScene(c); };
      cfg.AddProcessingRule("scene", scene_schema, load_scene);

      processed_stack.clear();
      int ret = cfg.ProcessItems(processed_stack);

      return ret;
    }
  };


};

#endif
