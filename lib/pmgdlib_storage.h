// P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib
#ifndef PMGDLIB_STORAGE_HH
#define PMGDLIB_STORAGE_HH 1

#include <string>
#include <unordered_map>
#include <memory>

#include "pmgdlib_std.h"

namespace pmgd {

  class Image;
  class Shader;
  class Scene;
  class FrameDrawer;
  class TextureDrawer;

  template<typename T> std::string add_type_prefix(const std::string & name){return "default:" + name;}
  template<> std::string add_type_prefix<Image>(const std::string & name){return "image:" + name;}
  template<> std::string add_type_prefix<Shader>(const std::string & name){return "shader:" + name;}
  template<> std::string add_type_prefix<Scene>(const std::string & name){return "scene:" + name;}
  template<> std::string add_type_prefix<FrameDrawer>(const std::string & name){return "frame_drawer:" + name;}
  template<> std::string add_type_prefix<TextureDrawer>(const std::string & name){return "texture_drawer:" + name;}

  class DataContainer : public BaseMsg {
    private:
    std::unordered_map<std::string, std::vector<std::string>> ids;
    std::unordered_map<std::string, std::shared_ptr<void>> data;

    public:
    template<typename T>
    std::vector<std::string> Ids(){
      std::string tkey = std::string(typeid(T).name());
      return unordered_map_get(ids, tkey, std::vector<std::string>());
    }

    template<typename T>
    std::vector<std::shared_ptr<T>> Objects(){
      std::vector<std::string> ids = Ids<T>();
      std::vector<std::shared_ptr<T>> objects;
      for(auto id : ids){
        objects.push_back(Get<T>(id));
      }
      return objects;
    }

    template<typename T>
    void AddIds(const std::string & id){
      std::string tkey = std::string(typeid(T).name());
      auto it = ids.find(tkey);
      if(it == ids.end()){
        ids[tkey].push_back(id);
        return;
      }
      it->second.push_back(id);
    }

    template<typename T>
    std::shared_ptr<T> Get(const std::string & name){
      if( not name.size() ) return nullptr;
      const std::string id = add_type_prefix<T>(name);
      std::unordered_map<std::string, std::shared_ptr<void>>::const_iterator ptr = data.find(id);
      if(ptr == data.end()) {
        msg_debug("can't find", quote(name), ", return nullptr");
        return nullptr;
      }
      msg_debug("find", quote(name), ", return it");
      return std::static_pointer_cast<T>(ptr->second);
    }

    template<typename T>
    int Add(const std::string & name, std::shared_ptr<T> obj){
      const std::string id = add_type_prefix<T>(name);
      std::unordered_map<std::string, std::shared_ptr<void>>::const_iterator ptr = data.find(id);
      if(ptr != data.end()) {
        msg_debug("can't add duplicate", quote(name));
        return PM_ERROR_DUPLICATE;
      }
      msg_debug("add", quote(name));
      data[id] = std::static_pointer_cast<void>(obj);
      AddIds<T>(name);
      return PM_SUCCESS;
    }
  };
}

#endif