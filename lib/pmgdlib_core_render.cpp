// P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib

#include "pmgdlib_core_render.h"

namespace pmgd {
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