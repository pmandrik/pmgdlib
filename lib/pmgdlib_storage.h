// P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib
#ifndef PMGDLIB_STORAGE_HH
#define PMGDLIB_STORAGE_HH 1

#include <string>
#include <unordered_map>
#include <memory>

#include "pmgdlib_defs.h"
#include "pmgdlib_msg.h"
#include "pmgdlib_string.h"
#include "pmgdlib_std.h"

namespace pmgd {
  class Image;
  class Shader;
  class Scene;
  class FrameDrawer;
  class TextureDrawer;

  template<typename T> std::string add_type_prefix(const std::string & name){
    return std::string(typeid(T).name()) + ":" + name;
  }

  //! DataContainer store ready to use objects
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
    std::vector<std::shared_ptr<T>> All(){
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
      msg_debug("find", quote(id), ", return it");
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
      msg_debug("add", quote(id));
      data[id] = std::static_pointer_cast<void>(obj);
      AddIds<T>(name);
      return PM_SUCCESS;
    }
  };

  //! internal tree to be used by NdMap 
  class NdMapTree {
    public:
    std::map<std::string, NdMapTree*> nodes;
    std::shared_ptr<void> data = nullptr;

    //! return tree node
    NdMapTree* Get(const std::string & part_id, bool add = false);
  };

  //! multi-level key
  class NdKey {
    unsigned int len = 0;
    std::unordered_map<int, std::string> data;

    public:
    NdKey(){};
    NdKey(std::string k){ Add(k); };
    NdKey(std::string k1, std::string k2){ Add(k1); Add(k2); };
    NdKey(std::initializer_list<std::string> il){ for(auto it : il){ Add(it); } };

    NdKey & operator += (const NdKey &k){
      for(int i = 0; i < k.size(); ++i) Add(k.at(i));
      return *this;
    }

    void Add(std::string key){ data[len++] = key; }
    unsigned int size() const { return len;}
    std::string at(unsigned int index) const { return data.find(index)->second; }
  };

  NdKey operator + (NdKey ka, NdKey kb);

  //! map that support access by multi-level NdKey
  template<typename T>
  class NdMap {
    NdMapTree head;

    std::vector<NdMapTree*>* GetHeads(const NdKey & key){
      std::vector<NdMapTree*> *heads = new std::vector<NdMapTree*>();
      std::vector<NdMapTree*> *next_heads = new std::vector<NdMapTree*>();
      heads->push_back(&head);
      
      for(int i = 0; i < key.size(); ++i){
        std::string part = key.at(i);

        for(int j = 0; j < heads->size(); ++j){
          NdMapTree* it = heads->at(j);
          if(part == "*"){
            for(auto node : it->nodes){
              next_heads->push_back(node.second);
            }
          } else {
            NdMapTree* next = it->Get(part);
            if(next == nullptr) continue;
            next_heads->push_back(next);
          }
        }

        std::swap(heads, next_heads);
        next_heads->clear();
      }
      delete next_heads;
      return heads;
    };

    public:
    NdMap(){};

    void Add(const NdKey & key, std::shared_ptr<T> data){
      NdMapTree * loc = &head;
      for(unsigned int i = 0; i < key.size(); ++i){
        msg(i, loc, key.at(i));
        loc = loc->Get(key.at(i), true);
      }
      loc->data = std::static_pointer_cast<void>(data);
    }

    std::vector<std::shared_ptr<T>> Get(const NdKey & key){
      std::vector<NdMapTree*>* heads = GetHeads(key);

      std::vector<std::shared_ptr<T>> answer;
      for(int j = 0; j < heads->size(); ++j){
        NdMapTree* it = heads->at(j);
        if(it->data == nullptr) continue;
        answer.push_back(std::static_pointer_cast<T>(it->data));
      }

      delete heads;
      return answer;
    }

    std::shared_ptr<T> GetOne(const NdKey & key){
      std::vector<NdMapTree*>* heads = GetHeads(key);
      std::shared_ptr<T> answer = nullptr;
      for(int j = 0; j < heads->size(); ++j){
        NdMapTree* it = heads->at(j);
        if(it->data == nullptr) continue;
        answer = std::static_pointer_cast<T>(it->data);
        break;
      }

      delete heads;
      return answer;
    }

    std::shared_ptr<T> GetOne(const std::vector<NdKey> & prefixs, const NdKey & postfix){
      std::shared_ptr<T> answer = nullptr;
      for(auto prefix: prefixs){
        NdKey key = prefix + postfix;
        answer = GetOne(key);
        if(answer != nullptr)
          return answer;
      }
      return answer;
    }
  };
}

#endif