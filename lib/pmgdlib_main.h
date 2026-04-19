// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721

#ifndef PMGDLIB_MAIN_HH
#define PMGDLIB_MAIN_HH 1

#include "pmgdlib_msg.h"
#include "pmgdlib_core.h"
#include "pmgdlib_config.h"
#include "pmgdlib_factory.h"
#include "pmgdlib_sdl.h"

/*
TARGET:

XML -> Config Wrapper -> ProtoObjects -> ColdObject -> WarmObject
Factory->Build(ProtoObject) -> GameObject
GameObject->WarmUp()

Object {
  WarmUp(){
  }

  CoolDown(){
    if () {
    }
  }
}

SceneMachine {
  CoolDownScene(scene) {
  }

  WarmUpScene(scene) {
  }

  SwitchScene(next_scene, transition rule) {
    next_scene
  }
};
*/

namespace pmgd {

  std::string gen_uuid(){
    static int counter = 0;
    return std::to_string(counter++);
  }

  void proto_objects_into_map(std::shared_ptr<NdMap<ProtoObject>> ndmap, const std::vector<std::shared_ptr<ProtoObject>> proto_objects){
    for(int i = 0; i < proto_objects.size(); ++i){
      std::shared_ptr<ProtoObject> po = proto_objects.at(i);
      
      // make key
      std::vector<const ConfigItem*> stack;
      const ConfigItem* head = po->cfg_item;
      while(head != nullptr){
        stack.push_back(head);
        head = head->father;
      }

      NdKey key;
      for(int i = stack.size() - 1; i >= 0; --i){
        const ConfigItem* cfg = stack[i];

        if(i != stack.size()-1)
          key.Add(cfg->type);
        key.Add(cfg->Attribute("id", gen_uuid()));
      }

      // add to tree
      ndmap->Add(key, po);
    }
  }

  class Main : public BaseMsg {
    SysOptions sysopts;
    std::shared_ptr<Backend> backend = nullptr;
    std::shared_ptr<DataContainer> dc = std::make_shared<DataContainer>();
    std::shared_ptr<NdMap<ProtoObject>> ndmap = std::make_shared<NdMap<ProtoObject>>();
    std::shared_ptr<ProtoObject> active_scene = nullptr, prev_scene = nullptr;

    int LoadCfgData(std::shared_ptr<Config> cfg){
      //! what objects load as ProtoObjects from cfg
      std::vector<std::string> proto_objects_keys = {"texture", "shader", "scene", "chain", "frame_drawer", "pipeline", "drawer"};

      //! step 1. load list of ProtoObjects 
      msg_debug("step 1. ...");
      ProtoLoader loader;
      int ret = loader.Load(cfg, proto_objects_keys);
      
      //! step 2. load ProtoObjects into NdMap
      msg_debug("step 2. ...");
      proto_objects_into_map(ndmap, loader.proto_objects);

      //! step 3. top level objects are warm, nested objects are cold
      msg_debug("step 3. ...");
      auto top_objects = ndmap->Get(NdKey({"*", "*", "*"}));
      for(auto item : top_objects){
        msg_debug("warm", quotec(item->cfg_item->type), "id =", quote(item->cfg_item->Attribute("id", "")));
      }

      std::vector<NdKey> namespaces = {NdKey({"default"}), NdKey("default")};
      ProtoBuilder pb(backend, ndmap);
      pb.BuildObjects(namespaces, top_objects);

      //! step 4. put scenes into container

      return ret;
    }

    void Setup(const std::string & cfg_raw){
      /// add config assuming it is with sys options
      auto cfg = AddCfg("default", cfg_raw);

      /// create backend using provided sys options
      msg_info("load system options ...");
      sysopts = get_cfg_sys_options(cfg);

      msg_info("load IO/accel backends ...");
      backend = get_backend(sysopts);
      if(not backend){
        msg_error("load IO/accel backends ... failed, return");
        return;
      }

      /// load objects
      msg_info("load data from cfg ...");
      LoadCfgData(cfg);

      /// maybe setup window & render & core
      auto w = backend->MakeWindow(sysopts);
      auto r = backend->MakeRender(sysopts, w);
      auto c = backend->MakeCore(sysopts);

      dc->Add("default", w);
      dc->Add("default", r);
      dc->Add("default", c);
    }

    public:
    Main(const std::string & cfg_raw){
      msg_info("Main() ... start");
      Setup(cfg_raw);
      msg_info("Main() ... done");
    }

    std::shared_ptr<Config> AddCfg(std::string key, const std::string & cfg_raw){
      msg_info("add config", quote(key));
      std::shared_ptr<Config> cfg = load_cfg(cfg_raw, key);
      if(cfg == nullptr){
        msg_error("load_cfg returns nullptr");
        return nullptr;
      }
      dc->Add(key, cfg);
      msg_info("add config", quotec(key), "done");
      return cfg;
    }

    void SetScene(std::string key){
      msg_info("set scene", quotec(key));
      auto scene = ndmap->GetOne(NdKey({"*", "scene", key}));
      if(not scene){
        msg_warning("can't find scene, skip");
        return;
      }
    }

    void Loop(){
      msg_info("Main Loop ... start");
      auto render = dc->Get<Render>("default");
      auto core = dc->Get<Core>("default");

      int x = 0;
      bool on = true;
      while(on){
        render->Draw();
        core->Tick();

        if(x++ > 1000) on = false;
      }
      msg_info("Main Loop ... done");
    }
  };
};

#endif
