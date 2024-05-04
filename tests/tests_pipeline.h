// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef TEST_PIPELINE_HH
#define TEST_PIPELINE_HH 1

#include "pmgdlib_pipeline.h"

using namespace pmgd;

void make_linear_graph(PipelineGraph & pg, int start_index = 0){
  // 0->1->2->3->4->5->6->7->8->9
  for(int i = 0; i < 10; i++)
    EXPECT_EQ(pg.AddNode(start_index+i), PM_SUCCESS);
  for(int i = 0; i < 9; i++)
    EXPECT_EQ(pg.AddEdge(start_index+i, start_index+i+1), PM_SUCCESS);
}

TEST(pmlib_pipeline, test_linear) {
  PipelineGraph pg;
  make_linear_graph(pg);

  std::vector<int*> pl = pg.GetPipeline();
  EXPECT_EQ(pl.size(), 10);
  for(int i = 0; i < (int)pl.size(); i++)
    EXPECT_EQ(*pl.at(i), i);

  // remove interface
  EXPECT_EQ(pg.RemoveNode(4), PM_SUCCESS);
  EXPECT_EQ(pg.RemoveEdge(3, 4), PM_SUCCESS);
  EXPECT_EQ(pg.RemoveEdge(4, 5), PM_SUCCESS);

  pl = pg.GetPipeline();
  EXPECT_EQ(pl.size(), 9);
}

TEST(pmlib_pipeline, test_multiple_linear) {
  PipelineGraph pg;
  int n_linears = 10;
  for(int i = 0; i < n_linears; i++)
    make_linear_graph(pg, 20 * i);

  EXPECT_EQ(pg.GetPipeline().size(), n_linears*10);
}

TEST(pmlib_pipeline, head_tail_loop) {
  PipelineGraph pg;
  make_linear_graph(pg);

  // add head-tail loop
  EXPECT_EQ(pg.AddEdge(9, 0), PM_SUCCESS);

  // return 0 size pipeline
  EXPECT_EQ(pg.GetPipeline().size(), 0);

  // remove loop
  EXPECT_EQ(pg.RemoveEdge(9, 0), PM_SUCCESS);
  EXPECT_EQ(pg.GetPipeline().size(), 10);
}

TEST(pmlib_pipeline, internal_loop) {
  PipelineGraph pg;
  make_linear_graph(pg);

  // add loop
  EXPECT_EQ(pg.AddEdge(7, 2), PM_SUCCESS);

  // return 0 size pipeline
  EXPECT_EQ(pg.GetPipeline().size(), 0);

  // remove loop
  EXPECT_EQ(pg.RemoveEdge(7, 2), PM_SUCCESS);
  EXPECT_EQ(pg.GetPipeline().size(), 10);
}

TEST(pmlib_pipeline, tree_graph) {
  PipelineGraph pg;

  // 1 <- 10, 11, 12
  // 10 <- 100, 101, 102
  // 11 <- 110, 111, 112
  // 12 <- 120, 121, 122
  EXPECT_EQ(pg.AddNode(1), PM_SUCCESS);
  for(int i = 0; i < 3; i++){
    int node_id = 10 + i;
    EXPECT_EQ(pg.AddNode(node_id), PM_SUCCESS);
    EXPECT_EQ(pg.AddEdge(node_id, 1), PM_SUCCESS);
    for(int j = 0; j < 3; j++){
      int sub_node_id = 100 + 10*i + j;
      EXPECT_EQ(pg.AddNode(sub_node_id), PM_SUCCESS);
      EXPECT_EQ(pg.AddEdge(sub_node_id, node_id), PM_SUCCESS);
    }
  }


}

#endif




