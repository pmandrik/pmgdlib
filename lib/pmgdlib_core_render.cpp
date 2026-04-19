// P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib

#include "pmgdlib_core_render.h"

namespace pmgd {
  //================================================ Base classes
  TexTile * TexAtlas::Get(const std::string & key){
    auto ptr = atlas.find(key);
    if(ptr == atlas.end()) return nullptr;
    return &(ptr->second);
  }

  void TexAtlas::Add(const std::string & key, const v2 & tp, const v2 & ts){
    if(Get(key)) return;
    atlas[key] = TexTile(tp, ts);
  }

  std::string TexAtlas::GenItemKey(int x, int y) const {
    return "X" + std::to_string(x) + "Y" + std::to_string(y);
  };

  std::string TexAtlas::GenItemKey(std::string name, int x, int y) const {
    return name + "_" + std::to_string(x) + "_" + std::to_string(y);
  };

  //================================================ Drawers
  unsigned int ArrayDrawer::FindFreePosition(){
    /// search free position in data array
    if(free_positions.size()) {
      unsigned int index = free_positions.top();
      free_positions.pop();
      return index;
    }
    for(unsigned int i = last_quad_id+1; i < max_quads_number; i++){
      if(IsFreeIndex(i)){ last_quad_id = i; return i; }
    }
    for(unsigned int i = 0; i <= last_quad_id; i++){
      if(IsFreeIndex(i)){ last_quad_id = i; return i; }
    }
    msg_warning("can't find free position");
    return GetDefaultId();
  }

  //================================================ Render
  void Render::SetPipeline(std::shared_ptr<RenderPipeline> pipeline_){
    pipeline = pipeline_;
  }

  void Render::Draw(){
    if(not pipeline) return;
    for(auto iter : pipeline->items){
      // iter->Draw();
    }
  }

};