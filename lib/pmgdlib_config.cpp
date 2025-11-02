// P.~Mandrik, 2025

#include <string>
#include <functional>
#include <vector>
#include <map>
#include <memory>

#include "pmgdlib_std.h"
#include "pmgdlib_msg.h"
#include "pmgdlib_core.h"
#include "pmgdlib_config.h"

namespace pmgd {
  // ======= ConfigItem ====================================================================
  void ConfigItem::AddAttribute(const std::string & name, const std::string & value)  { attributes[name] = value; }
  void ConfigItem::AddAttribute(std::string && name, std::string && value){ attributes[name] = value; }

  void ConfigItem::Add(std::string name, ConfigItem & value){
    nested[name].push_back(value);
  }

  bool ConfigItem::Merge(ConfigItem & other){
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
  bool ConfigItem::HasAttribute(std::string name) const {
    return attributes.find(name) != attributes.end();
  }

  /// Get Attribute as std::string, int, float
  std::string ConfigItem::Attribute(std::string name, std::string def) const {
    return map_get( attributes, name, def );
  }

  // std::string AttributeUpper(std::string name, std::string def = "") const {
  //   std::string answer = map_get( attributes, name, def );
  //   return upper_string(answer);
  // }

  int ConfigItem::AttributeI(std::string name, int def) const {
    std::string attr = Attribute( name );
    if(not attr.size()) return def;
    return atoi(attr.c_str()); //FIXME
  }

  float ConfigItem::AttributeF(std::string name, float def) const {
    std::string attr = Attribute( name );
    if(not attr.size()) return def;
    return atof(attr.c_str()); //FIXME
  }

  /// Get ConfigItem from nested
  std::vector<ConfigItem> ConfigItem::Get(std::string name) const {
    return map_get(nested, name, std::vector<ConfigItem>());
  }

  std::vector<std::string> ConfigItem::GetAttrsFromNested(std::string name, std::string attr) const {
    /// Get ConfigItem from nested
    std::vector<std::string> answer;
    std::vector<ConfigItem> rels = Get(name);
    for(auto rel : rels){
    answer.push_back(rel.Attribute(attr));
    }
    return answer;
  }

  void ConfigItem::FillHrString(std::string & hr, int n_tabs, int max_depth) const {
    std::string ntabs = std::string(n_tabs, ' ');
    std::string ntabs2 = std::string(n_tabs+2, ' ');

    if(attributes.size()){
    hr += ntabs + "attributes:\n";
    for(auto iter : attributes)
        hr += ntabs2 + iter.first + "=\"" + iter.second + "\"\n";
    }

    if(nested.size() == 0) return;
    hr += ntabs + "nested data:\n";
    if(max_depth == 0){
    hr += "...";
    return;
    }

    for(auto iter : nested){
    hr += ntabs2 + iter.first + "'s:\n";
    for(auto item : iter.second) item.FillHrString(hr, n_tabs + 4, max_depth-1);
    }
  }

  std::string ConfigItem::AsString(int max_depth) const {
    std::string hr;
    FillHrString(hr, 0, max_depth);
    return hr;
  }

  // ======= Config ====================================================================
  int Config::ProcessNestedItems(const std::map<std::string, std::vector<ConfigItem>> & nested, std::vector<const ConfigItem*> & processed_stack) const {
    /// call ProcessItem to every provided nested item
    int tot_ret = PM_SUCCESS;
    for(auto iter = nested.begin(); iter != nested.end(); ++iter){
      const std::string & key = iter->first;
      const std::vector<ConfigItem> & group = iter->second;

      /// nested items form groups, every group has specific processing rule
      auto rule = map_get(processing_rules, key, default_processing_rule);
      // int ret = ProcessNestedGroup(group, rule, processed_stack);
      for(int i = 0, i_max = group.size(); i < i_max; ++i){
        int ret = ProcessItem(group[i], rule, processed_stack);
        tot_ret = (ret == PM_SUCCESS ? tot_ret : ret);

        if(ret == PM_ERROR_SCHEMA){msg_warning("item = ", i, "invalid schema = ", ret);}
        else if(ret != PM_SUCCESS){msg_warning("item = ", i, "processed with error code = ", ret);}
      }
    }
    return tot_ret;
  }

  int Config::ProcessItem(const ConfigItem & item, const ConfigProcessingRule & rule, std::vector<const ConfigItem*> & processed_stack) const {
    int valid = Validate(rule.schema, item, processed_stack);
    if(valid != PM_SUCCESS) return valid;
    int status = rule.proccessor(item);
    if(status != PM_SUCCESS) return status;

    processed_stack.push_back(&item);
    int ret = ProcessNestedItems(item.nested, processed_stack);
    processed_stack.pop_back();
    return ret;
  }

  int Config::Validate(const ConfigSchema & schema, const ConfigItem & item, std::vector<const ConfigItem*> & processed_stack) const {
    for(auto mand : schema.mandatory){
      std::string val = item.Attribute(mand);
      if(val.size() == 0){
        std::string str = item.AsString();
        msg_warning("item", quote(str), "has no mandatory attribute \"" + mand + "\"" );
        return PM_ERROR_SCHEMA;
      }
    }

    if(processed_stack.size() < schema.fathers.size()){
      std::string str = item.AsString();
      msg_warning("item", quote(str), "nested depth requirements =", schema.fathers.size(), "> stack size =", processed_stack.size() );
      return PM_ERROR_SCHEMA;
    }

    int depth = 1;
    for(auto father : schema.fathers){
      const ConfigItem * cfg = processed_stack.at(schema.fathers.size() - depth);
      if(cfg->type != father){
        std::string str = item.AsString();
        msg_warning(quote(str), "at depth level ", depth, " requires father", quote(father), " != ", quote(cfg->type));
        return PM_ERROR_SCHEMA;
      }
      depth += 1;
    }

    return PM_SUCCESS;
  }

  //! add schema and function to validate and process config item element
  void Config::AddProcessingRule(std::string key, const ConfigSchema & schema, std::function<int(const ConfigItem&)> proccessor) {
    processing_rules[key] = ConfigProcessingRule(schema, proccessor);
  }

  int Config::ProcessItems(std::vector<const ConfigItem*> & processed_stack) const {
    /// top element do not have attributes, thus, this is directly alias over ProcessNestedItems
    return ProcessNestedItems(this->nested, processed_stack);
  }

  std::string Config::ProcessTemplate(std::string raw) const {
    /// TODO
    return raw;
  }

  std::string Config::IdFromPath(std::string & path) const {
    /// TODO
    return path;
  }

  // ======= ConfigLoader ====================================================================
  Config ConfigLoader::LoadXmlCfg(const std::string & raw){
    Config cfg;
    return cfg;
  }

  SysOptions ConfigLoader::GetSysOptions(const Config & cfg){
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