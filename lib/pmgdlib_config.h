// P.~Mandrik, 2025

#ifndef PMLIB_CONFIG_HH
#define PMLIB_CONFIG_HH 1

#include <string>
#include <functional>
#include <vector>
#include <map>

#include "pmgdlib_std.h"
#include "pmgdlib_msg.h"
#include "pmgdlib_core.h"
#include "pmgdlib_string.h"

#ifdef USE_TINYXML2
  #include "tinyxml2.h"
#endif

namespace pmgd {
  // ======= config item ====================================================================
  struct ConfigItem : public BaseMsg {
    /// Store nested to create Shta classes, something like:
    /// type, parameters -> item = new type(parameters['name1'], parameters['name2'], ... )
    /// item.field[0] = nested['filed'][0] etc
    std::string type;
    std::map<std::string, std::string> attributes;
    std::map<std::string, std::vector<ConfigItem*>> nested;
    bool valid = true;
    ConfigItem *father = nullptr;

    /// Add Attribute & Data
    void AddAttribute(const std::string & name, const std::string & value);
    void AddAttribute(std::string && name, std::string && value);
    void Add(std::string name, ConfigItem *value);

    /// Merge with another cfg
    bool Merge(ConfigItem * other);

    /// check if has attribute
    bool HasAttribute(std::string name) const;

    /// Get Attribute as std::string, int, float
    std::string Attribute(std::string name, std::string def = "") const;

    // std::string AttributeUpper(std::string name, std::string def = "") const
    int AttributeI(std::string name, int def = 0) const;
    float AttributeF(std::string name, float def = 0.f) const;

    /// Get ConfigItem from nested
    std::vector<ConfigItem*> Get(std::string name) const;
    std::vector<std::string> GetAttrsFromNested(std::string name, std::string attr) const;

    /// Create HR format for printing
    void FillHrString(std::string & hr, int n_tabs = 0, int max_depth = -1) const;
    std::string AsString(int max_depth = 0) const;
  };

  // ======= config ====================================================================
  struct ConfigSchema {
    std::vector <std::string> mandatory;
    std::vector <std::string> fathers;
    ConfigSchema(){}
    ConfigSchema(std::vector <std::string> mandatory, std::vector <std::string> fathers={}): mandatory(mandatory), fathers(fathers){};
  };

  using ConfigProccessor = std::function<int(const ConfigItem*)>;
  struct ConfigProcessingRule {
    ConfigSchema schema;
    ConfigProccessor proccessor;
    ConfigProcessingRule(const ConfigSchema & schema, ConfigProccessor proccessor): schema(schema), proccessor(proccessor){};
    ConfigProcessingRule(ConfigProccessor proccessor): proccessor(proccessor){};
    ConfigProcessingRule(){proccessor = [](const ConfigItem* c) { return PM_SUCCESS; };}
  };

  class Config : public ConfigItem {
    std::map<std::string, ConfigProcessingRule> processing_rules;
    ConfigProcessingRule default_processing_rule;

    int ProcessNestedGroup(const std::vector<ConfigItem*> & group, const ConfigProcessingRule & rule, 
      std::vector<const ConfigItem*> & processed_stack) const;

    int ProcessNestedItems(const std::map<std::string, std::vector<ConfigItem*>> & nested, 
      std::vector<const ConfigItem*> & processed_stack) const;

    int ProcessItem(const ConfigItem* item, const ConfigProcessingRule & rule, 
      std::vector<const ConfigItem*> & processed_stack) const;

    public:
    int Validate(const ConfigSchema & schema, const ConfigItem* item, 
      std::vector<const ConfigItem*> & processed_stack) const;

    //! add schema and function to validate and process config item element
    void AddProcessingRule(std::string key, const ConfigSchema & schema, std::function<int(const ConfigItem*)> proccessor);
    void AddProcessingRule(std::string key, std::function<int(const ConfigItem*)> proccessor);

    int ProcessItems(std::vector<const ConfigItem*> & processed_stack) const;

    std::string ProcessTemplate(std::string raw) const;

    std::string IdFromPath(std::string & path) const;
  };

  // ======= config loading implementation ====================================================================
  class ConfigLoaderImp : public BaseMsg {
      public:
      virtual ~ConfigLoaderImp(){}
      /// predefined function to load data into internal cfg format
      virtual void ToCfg(const std::string & raw, std::shared_ptr<Config> cfg, std::string id = "") = 0;
  };

  #ifdef USE_TINYXML2
    class TinyXmlConfigLoaderImp : public ConfigLoaderImp {
      tinyxml2::XMLDocument doc;

      void ToCfgRec(ConfigItem* item, const tinyxml2::XMLElement* head);

      public:
      virtual ~TinyXmlConfigLoaderImp(){}
      virtual void ToCfg(const std::string & raw, std::shared_ptr<Config> cfg, std::string id = ""){
        doc.Parse(raw.c_str());

        cfg->type = "cfg";
        cfg->AddAttribute("id", id);
        for(const tinyxml2::XMLElement* child = doc.FirstChildElement(); child; child = child->NextSiblingElement()) {
          const char* name = child->Name();
          ConfigItem *child_item = new ConfigItem();
          child_item->type = name;
          ToCfgRec(child_item, child);
          cfg->Add(name, child_item);
        }
      }
    };
  #endif

  // ======= ProtoObject ====================================================================
  class ProtoObject : public BaseMsg {
    public:
    const ConfigItem * cfg_item;
    std::shared_ptr<void> object = nullptr;

    ProtoObject(const ConfigItem * cfg_item_): cfg_item(cfg_item_) {}
    bool IsWarm(){ return object == nullptr; }
  };

  //! get Config as input and setup ProtoObjects
  class ProtoLoader : public BaseMsg {
    std::string DUMMY_KEY = "";

    // process stack
    std::vector<const ConfigItem*> processed_stack;

    int LoadProtoObject(const ConfigItem* cfg);

    public:
    ProtoLoader(){}

    // actual loading of ProtoObjects with type in <keys> from <cfg>
    int Load(std::shared_ptr<Config> cfg, const std::vector<std::string> & keys);
    std::vector<std::shared_ptr<ProtoObject>> proto_objects;
  };  

  // ======= functions to use outside ====================================================================
  //! use this function to load raw cfg into Config class with ConfigItems
  std::shared_ptr<Config> load_cfg(const std::string & raw, const std::string id);

  //! get base engine sys options from config
  SysOptions get_cfg_sys_options(std::shared_ptr<Config> cfg);
};

#endif