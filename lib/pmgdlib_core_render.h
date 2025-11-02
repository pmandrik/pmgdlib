// P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib

#ifndef PMGDLIB_CORE_RENDER_HH
#define PMGDLIB_CORE_RENDER_HH 1

#include <list>
#include <memory>
#include "pmgdlib_msg.h"

namespace pmgd {

  class Drawable {
    public:
    virtual ~Drawable(){};
    virtual void Draw(Drawable * target){};
  };

  class RenderPipelineItem {
    public:
    bool new_target = true, new_source = true, cleanup_source = true, cleanup_target = true;
    Drawable * target;
    Drawable * source;

    void Draw(){
      source->Draw(target);
    }
  };

  class RenderPipeline {
    public:
    std::vector<RenderPipelineItem> items;
    void AddItem(RenderPipelineItem item){ items.push_back(item); }
  };

  class Render : public BaseMsg {
    std::shared_ptr<RenderPipeline> pipeline;

    public:
    void SetPipeline(std::shared_ptr<RenderPipeline> pipeline_);
    void Draw();
    virtual ~Render(){};
  };

  // XML -> graph -> pipeline vector
  // pipeline vector + drawable objects -> RenderPipeline -> Render

};

#endif