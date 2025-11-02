// P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib

#ifndef TEST_CORE_HH
#define TEST_CORE_HH 1

#include "pmgdlib_core.h"
#include "pmgdlib_core_render.h"
#include "pmgdlib_factory.h"

using namespace pmgd;

bool str_to_renderpipeline_check(auto pipeline, int i, std::shared_ptr<DataContainer> dc, std::string source, std::string target){
  RenderPipelineItem & pi = pipeline->items.at(i);
  std::shared_ptr<Drawable> s = dc->Get<Drawable>(source);
  std::shared_ptr<Drawable> t = dc->Get<Drawable>(target);

  // msg(pi.source, s.get(), pi.target, t.get());
  return pi.source == s.get() && pi.target == t.get();
}

TEST(pmlib_core, str_to_renderpipeline) {
  // prep
  vector<string> elements = {"bg", "window", "aqd1", "aqd2", "ps"};
  auto dc = std::make_shared<DataContainer>();
  for(auto el : elements){
    dc->Add(el, std::make_shared<Drawable>());
  }

  // code
  Builder bd;
  bd.SetDataSource(dc);
  string chain = "bg->window, aqd1->window, aqd2->window, ps->window";
  auto graph = bd.BuildGraph(chain);
  std::shared_ptr<RenderPipeline> pipeline = bd.BuildRenderPipeline(graph);

  // check
  EXPECT_EQ(pipeline->items.size(), 4);
  EXPECT_TRUE(str_to_renderpipeline_check(pipeline, 0, dc, "bg", "window"));
  EXPECT_TRUE(str_to_renderpipeline_check(pipeline, 1, dc, "aqd1", "window"));
  EXPECT_TRUE(str_to_renderpipeline_check(pipeline, 2, dc, "aqd2", "window"));
  EXPECT_TRUE(str_to_renderpipeline_check(pipeline, 3, dc, "ps", "window"));
}

#endif