// P.~Mandrik, 2025

#ifndef PMLIB_CONFIG_HH
#define PMLIB_CONFIG_HH 1

#include "pmgdlib_std.h"
#include "pmgdlib_msg.h"
#include "pmgdlib_core.h"

#include "tinyxml2.h"

namespace pmgd {
  // ======= config ====================================================================
  struct ConfigItem : public BaseMsg {
    /// Store data to create Shta classes, something like:
    /// type, parameters -> item = new type(parameters['name1'], parameters['name2'], ... )
    /// item.field[0] = data['filed'][0] etc
    std::string type;
    std::map<std::string, std::string> attributes;
    std::map<std::string, std::vector<ConfigItem>> data;
    bool valid = true;

    /// Add Attribute & Data
    void AddAttribute(const std::string & name, const std::string & value)  { attributes[name] = value; }
    void AddAttribute(std::string && name, std::string && value){ attributes[name] = value; }

    void Add(std::string name, ConfigItem & value){
      data[name].push_back(value);
    }

    bool Merge(ConfigItem & other){
      /// collision policy - overwrite using values from 'other'
      for(auto iter = other.attributes.begin(); iter != other.attributes.end(); ++iter)
        AddAttribute(iter->first, iter->second);
      
      for(auto iter = other.data.begin(); iter != other.data.end(); ++iter){
        for(auto data_iter = iter->second.begin(); data_iter != iter->second.begin(); ++data_iter)
          Add(iter->first, *data_iter);
      }
      return PM_SUCCESS;
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

    /// Get ConfigItem from Data
    vector<ConfigItem> Get(std::string name) const {
      return map_get(data, name, std::vector<ConfigItem>());
    }

    vector<std::string> GetAttrsFromData(std::string name, std::string attr) const {
      /// Get ConfigItem from Data
      std::vector<std::string> answer;
      std::vector<ConfigItem> rels = Get(name);
      for(auto rel : rels){
        answer.push_back(rel.Attribute(attr));
      }
      return answer;
    }

    void FillHrString(std::string & hr, int n_tabs = 0) const {
      std::string ntabs = std::string(n_tabs, ' ');
      std::string ntabs2 = std::string(n_tabs+2, ' ');
      hr += ntabs + "attributes:\n";
      for(auto iter : attributes)
        hr += ntabs2 + " " + iter.first + " " + iter.second + "\n";

      hr += ntabs + "child data:";
      for(auto iter : data){
        hr += ntabs2 + " " + iter.first + ":";
        for(auto item : iter.second) item.FillHrString(hr, n_tabs + 2);
      }
    }
  };

  class Config : public ConfigItem {
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
  };

};

#endif