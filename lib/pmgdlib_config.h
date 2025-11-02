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
  // ======= config ====================================================================
  struct ConfigItem : public BaseMsg {
    /// Store nested to create Shta classes, something like:
    /// type, parameters -> item = new type(parameters['name1'], parameters['name2'], ... )
    /// item.field[0] = nested['filed'][0] etc
    std::string type;
    std::map<std::string, std::string> attributes;
    std::map<std::string, std::vector<ConfigItem>> nested;
    bool valid = true;

    /// Add Attribute & Data
    void AddAttribute(const std::string & name, const std::string & value);
    void AddAttribute(std::string && name, std::string && value);
    void Add(std::string name, ConfigItem & value);

    /// Merge with another cfg
    bool Merge(ConfigItem & other);

    /// check if has attribute
    bool HasAttribute(std::string name) const;

    /// Get Attribute as std::string, int, float
    std::string Attribute(std::string name, std::string def = "") const;
    // std::string AttributeUpper(std::string name, std::string def = "") const
    int AttributeI(std::string name, int def = 0) const;
    float AttributeF(std::string name, float def = 0.f) const;

    /// Get ConfigItem from nested
    std::vector<ConfigItem> Get(std::string name) const;
    std::vector<std::string> GetAttrsFromNested(std::string name, std::string attr) const;

    /// Create HR format for printing
    void FillHrString(std::string & hr, int n_tabs = 0, int max_depth = -1) const;
    std::string AsString(int max_depth = 0) const;
  };

  struct ConfigSchema {
    std::vector <std::string> mandatory;
    std::vector <std::string> fathers;
    ConfigSchema(){}
    ConfigSchema(std::vector <std::string> mandatory, std::vector <std::string> fathers={}): mandatory(mandatory), fathers(fathers){};
  };

  using ConfigProccessor = std::function<int(const ConfigItem&)>;
  struct ConfigProcessingRule {
    ConfigSchema schema;
    ConfigProccessor proccessor;
    ConfigProcessingRule(const ConfigSchema & schema, ConfigProccessor proccessor): schema(schema), proccessor(proccessor){};
    ConfigProcessingRule(){proccessor = [](const ConfigItem & c) { return PM_SUCCESS; };}
  };

  class Config : public ConfigItem {
    std::map<std::string, ConfigProcessingRule> processing_rules;
    ConfigProcessingRule default_processing_rule;

    int ProcessNestedGroup(const std::vector<ConfigItem> & group, const ConfigProcessingRule & rule, 
      std::vector<const ConfigItem*> & processed_stack) const;

    int ProcessNestedItems(const std::map<std::string, std::vector<ConfigItem>> & nested, 
      std::vector<const ConfigItem*> & processed_stack) const;

    int ProcessItem(const ConfigItem & item, const ConfigProcessingRule & rule, 
      std::vector<const ConfigItem*> & processed_stack) const;

    public:
      
    int Validate(const ConfigSchema & schema, const ConfigItem & item, 
      std::vector<const ConfigItem*> & processed_stack) const;

    //! add schema and function to validate and process config item element
    void AddProcessingRule(std::string key, const ConfigSchema & schema, std::function<int(const ConfigItem&)> proccessor);

    int ProcessItems(std::vector<const ConfigItem*> & processed_stack) const;

    std::string ProcessTemplate(std::string raw) const;

    std::string IdFromPath(std::string & path) const;
  };

  // ======= config loading ====================================================================
  class ConfigLoader : public BaseMsg {
    public:
    virtual Config LoadXmlCfg(const std::string & raw);
    SysOptions GetSysOptions(const Config & cfg);
  };

  #ifdef USE_TINYXML2
    class TinyXmlConfigLoader : public ConfigLoader {
      tinyxml2::XMLDocument doc;

      void ToCfgRec(ConfigItem & item, const tinyxml2::XMLElement* head){
        /// add attributes
        for (const tinyxml2::XMLAttribute* attr = head->FirstAttribute(); attr; attr = attr->Next()) {
          const char* name = attr->Name();
          const char* value = attr->Value();
          item.AddAttribute(name, value);
        }

        /// add childs
        for (const tinyxml2::XMLElement* child = head->FirstChildElement(); child; child = child->NextSiblingElement()) {
          const char* name = child->Name();
          ConfigItem child_item ;
          child_item.type = name;
          ToCfgRec(child_item, child);
          item.Add(name, child_item);
        }
      }

      public:
      /// predefined function to load xml into internal cfg format
      void ToCfg(Config & cfg){
        for (const tinyxml2::XMLElement* child = doc.FirstChildElement(); child; child = child->NextSiblingElement()) {
          const char* name = child->Name();
          ConfigItem child_item;
          child_item.type = name;
          ToCfgRec(child_item, child);
          cfg.Add(name, child_item);
        }
      }

      virtual Config LoadXmlCfg(const std::string & raw){
        Config cfg;
        doc.Parse(raw.c_str());
        ToCfg(cfg);
        return cfg;
      }
    };
  #endif
};

#endif