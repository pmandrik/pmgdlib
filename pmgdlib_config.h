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

#include "tinyxml2.h"

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
    void AddAttribute(const std::string & name, const std::string & value)  { attributes[name] = value; }
    void AddAttribute(std::string && name, std::string && value){ attributes[name] = value; }

    void Add(std::string name, ConfigItem & value){
      nested[name].push_back(value);
    }

    bool Merge(ConfigItem & other){
      /// collision policy - overwrite using values from 'other'
      for(auto iter = other.attributes.begin(); iter != other.attributes.end(); ++iter)
        AddAttribute(iter->first, iter->second);
      
      for(auto iter = other.nested.begin(); iter != other.nested.end(); ++iter){
        for(auto data_iter = iter->second.begin(); data_iter != iter->second.begin(); ++data_iter)
          Add(iter->first, *data_iter);
      }
      return PM_SUCCESS;
    }

    /// check if has attribute
    bool HasAttribute(std::string name) const {
      return attributes.find(name) != attributes.end();
    }

    /// Get Attribute as std::string, int, float
    std::string Attribute(std::string name, std::string def = "") const {
      return map_get( attributes, name, def );
    }

    // std::string AttributeUpper(std::string name, std::string def = "") const {
    //   std::string answer = map_get( attributes, name, def );
    //   return upper_string(answer);
    // }

    int AttributeI(std::string name, int def = 0) const {
      std::string attr = Attribute( name );
      if(not attr.size()) return def;
      return atoi(attr.c_str()); //FIXME
    }

    float AttributeF(std::string name, float def = 0.f) const {
      std::string attr = Attribute( name );
      if(not attr.size()) return def;
      return atof(attr.c_str()); //FIXME
    }

    /// Get ConfigItem from nested
    std::vector<ConfigItem> Get(std::string name) const {
      return map_get(nested, name, std::vector<ConfigItem>());
    }

    std::vector<std::string> GetAttrsFromNested(std::string name, std::string attr) const {
      /// Get ConfigItem from nested
      std::vector<std::string> answer;
      std::vector<ConfigItem> rels = Get(name);
      for(auto rel : rels){
        answer.push_back(rel.Attribute(attr));
      }
      return answer;
    }

    void FillHrString(std::string & hr, int n_tabs = 0, int max_depth = -1) const {
      std::string ntabs = std::string(n_tabs, ' ');
      std::string ntabs2 = std::string(n_tabs+2, ' ');
      hr += ntabs + "attributes:\n";
      for(auto iter : attributes)
        hr += ntabs2 + " " + iter.first + " " + iter.second + "\n";

      hr += ntabs + "nested data:\n";
      if(max_depth == 0){
        hr += "...";
        return;
      }

      for(auto iter : nested){
        hr += ntabs2 + " " + iter.first + ":";
        for(auto item : iter.second) item.FillHrString(hr, n_tabs + 2, max_depth-1);
      }
    }

    std::string GetShortStr(void) const {
      std::string hr;
      FillHrString(hr, 0, 0);
      return hr;
    }
  };

  struct ConfigSchema {
    std::vector <std::string> mandatory;
    std::vector <std::string> fathers;
    ConfigSchema(){}
    ConfigSchema(std::vector <std::string> mandatory, std::vector <std::string> fathers={}){
      this->mandatory = mandatory;
      this->fathers = fathers;
    }
  };

  using ConfigProccessor = std::function<int(const ConfigItem&)>;
  struct ConfigProcessingRule {
    ConfigSchema schema;
    ConfigProccessor proccessor;

    ConfigProcessingRule(){}
    ConfigProcessingRule(const ConfigSchema & schema, ConfigProccessor proccessor){
      this->schema = schema;
      this->proccessor = proccessor;
    }
  };

  class Config : public ConfigItem {
    std::map<std::string, ConfigProcessingRule> processing_rules;

    int ProcessNestedItems(const std::map<std::string, std::vector<ConfigItem>> & nested, std::vector<const ConfigItem*> & processed_stack) const {
      for(auto iter = nested.begin(); iter != nested.end(); ++iter){
        const std::string & key = iter->first;
        auto nested_rule = processing_rules.find(key);
        if(nested_rule == processing_rules.end()) continue;

        const std::vector<ConfigItem> & group = iter->second;
        for(int i = 0, i_max = group.size(); i < i_max; ++i){
          int ret = ProcessItem(group[i], nested_rule->second, processed_stack);
          if(ret == PM_ERROR_SCHEMA){msg_warning("item = ", key, i, "invalid schema = ", ret);}
          else if(ret != PM_SUCCESS){msg_warning("item = ", key, i, "processed with error code = ", ret);}

          if(ret != PM_SUCCESS) return ret;
        }
      }
      return PM_SUCCESS;
    }

    int ProcessItem(const ConfigItem & item, const ConfigProcessingRule & rule, std::vector<const ConfigItem*> & processed_stack) const {
      int valid = Validate(rule.schema, item, processed_stack);
      if(valid != PM_SUCCESS) return valid;

      int status = rule.proccessor(item);
      if(status != PM_SUCCESS) return status;

      processed_stack.push_back(&item);
      int ret = ProcessNestedItems(item.nested, processed_stack);
      processed_stack.pop_back();
      return ret;
    }

    public:
    int Validate(const ConfigSchema & schema, const ConfigItem & item, std::vector<const ConfigItem*> & processed_stack) const {
      for(auto mand : schema.mandatory){
        std::string val = item.Attribute(mand);
        if(val.size() == 0){
          std::string str = item.GetShortStr();
          msg_warning("item", quote(str), "has no mandatory attribute \"" + mand + "\"" );
          return PM_ERROR_SCHEMA;
        }
      }

      if(processed_stack.size() < schema.fathers.size()){
        std::string str = item.GetShortStr();
        msg_warning("item", quote(str), "nested depth requirements =", schema.fathers.size(), "> stack size =", processed_stack.size() );
        return PM_ERROR_SCHEMA;
      }

      int depth = 0;
      for(auto father : schema.fathers){
        const ConfigItem * cfg = processed_stack.at(schema.fathers.size() - depth);
        if(cfg->type != father){
          std::string str = item.GetShortStr();
          msg_warning(quote(str), "require father", quote(father), "at depth level != ", cfg->type);
          return PM_ERROR_SCHEMA;
        }
        depth += 1;
      }

      return PM_SUCCESS;
    }

    //! add schema and function to validate and process config item element
    void AddProcessingRule(std::string key, const ConfigSchema & schema, std::function<int(const ConfigItem&)> proccessor) {
      processing_rules[key] = ConfigProcessingRule(schema, proccessor);
    }

    int ProcessItems(std::vector<const ConfigItem*> & processed_stack) const {
      /// top element do not have attributes, thus, this is directly alias over ProcessNestedItems
      return ProcessNestedItems(this->nested, processed_stack);
    }

    std::string ProcessTemplate(std::string raw) const {
      /// TODO
      return raw;
    }

    std::string IdFromPath(std::string & path) const {
      /// TODO
      return path;
    }
  };

  // ======= config loading ====================================================================
  class TinyXML {
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
        ToCfgRec(child_item, child);
        cfg.Add(name, child_item);
      }
    }

    TinyXML(const std::string & raw){
      doc.Parse(raw.c_str());
      /// TODO
      /*
      XMLError error = doc.Parse(raw.c_str());
      if( error ){
        check_tinyxml_error(error, cfg_path);
        state = 1;
        return false;
      }
      */
    }
  };

  class ConfigLoader : public BaseMsg {
    public:
    Config LoadXmlCfg(const std::string & raw){
      Config cfg;
      TinyXML txml = TinyXML(raw);
      txml.ToCfg(cfg);
      return cfg;
    }

    SysOptions GetSysOptions(const Config & cfg){
      SysOptions sysopt;
      std::vector<ConfigItem> items = cfg.Get("sys");
      for(auto item : items){
        if(item.HasAttribute("screen_width")) sysopt.screen_width = item.AttributeI("screen_width");
        if(item.HasAttribute("screen_height")) sysopt.screen_height = item.AttributeI("screen_height");
        if(item.HasAttribute("multimedia_library")) sysopt.multimedia_library = item.Attribute("multimedia_library");
        if(item.HasAttribute("accelerator")) sysopt.accelerator = item.Attribute("accelerator");
        if(item.HasAttribute("io_backend")) sysopt.io = item.Attribute("io_backend");
        if(item.HasAttribute("img_backend")) sysopt.img = item.Attribute("img_backend");
      }
      return sysopt;
    }
  };

};

#endif