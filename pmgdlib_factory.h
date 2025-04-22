// P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib
#ifndef PMGDLIB_FACTORY_HH
#define PMGDLIB_FACTORY_HH 1

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
    std::shared_ptr<AccelFactory> accel_imp  = std::make_shared<AccelFactory>();
    std::shared_ptr<Texture> MakeTexture(std::shared_ptr<Image> img){ return accel_imp->MakeTexture(img); }
    std::shared_ptr<Shader> MakeShader(const std::string & vert_txt, const std::string & frag_txt){
      return accel_imp->MakeShader(vert_txt, frag_txt);
    }
  };

  class Backend {
    public:
    std::shared_ptr<IO> io;
    std::shared_ptr<Factory> factory;
  };

  struct BackendOptions {
    std::string accelerator;
    std::string io;
    std::string img;
  };

  Backend get_backend(BackendOptions options){
    std::shared_ptr<IO> io = std::make_shared<IO>();
    io->txt_imp = std::make_shared<IoTxt>();

    std::shared_ptr<Factory> factory = std::make_shared<Factory>();
    factory->accel_imp  = std::make_shared<AccelFactory>();

    Backend back;
    back.io = io;
    back.factory = factory;

    #ifdef USE_GL
    if(options.accelerator == "GL"){
      factory->accel_imp = std::make_shared<AccelFactoryGL>();
    }
    #endif

    #ifdef USE_SDL
    if(options.io == "SDL"){
      io->txt_imp = std::make_shared<IoTxtSDL>();
    }
    #endif

    #ifdef USE_STB
    if(options.img == "STB"){
      io->img_imp = std::make_shared<IoImageStb>();
    }
    #endif

    return back;
  };

  class DataContainer : public BaseMsg {
    private:
    std::unordered_map<std::string, std::shared_ptr<void>> data;

    public:
    template<typename T>
    std::shared_ptr<T> Get(const std::string & name){
      if( not name.size() ) return nullptr;
      std::unordered_map<std::string, std::shared_ptr<void>>::const_iterator ptr = data.find(name);
      if(ptr == data.end()) {
        msg_warning("can't find", name, ", return nullptr");
        return nullptr;
      }
      return std::static_pointer_cast<T>(ptr->second);
    }

    template<typename T>
    int Add(const std::string & name, std::shared_ptr<T> obj){
      std::unordered_map<std::string, std::shared_ptr<void>>::const_iterator ptr = data.find(name);
      if(ptr != data.end()) {
        msg_warning("can't add duplicate", name);
        return PM_ERROR_DUPLICATE;
      }
      msg_debug("add", name);
      data[name] = std::static_pointer_cast<void>(obj);
      return PM_SUCCESS;
    }
  };

  class SceneDataLoader : public BaseMsg {
    int LoadImage(std::string id, std::string path){
      msg_debug("load image, id = ", id, "path = ", path);
  
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
        msg_debug("texture image is passed by path", image_path);
        int err = LoadImage(image_id, image_path);
        if(err != PM_SUCCESS){
          msg_warning("texture image load failed", image_path);
          return err;
        }
      } else msg_debug("texture image is passed by id", image_id);

      /// build texture
      std::shared_ptr<Image> img = dc.Get<Image>(image_id);
      if(img == nullptr){
        msg_warning("can't find image", image_id);
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

    std::shared_ptr<IO> io;
    std::shared_ptr<Factory> factory;
    DataContainer dc;

    public:
    SceneDataLoader(const Backend & backend){
      io = backend.io;
      factory = backend.factory;
    }

    int Load(Config & cfg){
      ConfigSchema texture_schema({"id"});
      std::function<int(ConfigItem&)> load_texture = [this](const ConfigItem & c) { return this->LoadTexture(c); };
      cfg.ProcessItems("texture", texture_schema, load_texture);

      ConfigSchema shader_schema({"id"});
      std::function<int(ConfigItem&)> load_shader = [this](const ConfigItem & c) { return this->LoadShader(c); };
      cfg.ProcessItems("shader", shader_schema, load_shader);

      return PM_SUCCESS;
    }
  };


};

#endif
