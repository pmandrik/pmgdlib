// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef TEST_PIPELINE_HH
#define TEST_PIPELINE_HH 1

#include "pmgdlib_pipeline.h"

using namespace pmgd;

void make_linear_graph(PipelineGraph<int> & pg, int start_index = 0){
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

TEST(pmlib_pipeline, one_to_many_1) {
  PipelineGraph pg;
  EXPECT_EQ(pg.AddNode(10), PM_SUCCESS);

  // 10 -> 1, 2, 3, 4, 5
  for(int i = 0; i < 5; i++){
    EXPECT_EQ(pg.AddNode(i + 1), PM_SUCCESS);
    EXPECT_EQ(pg.AddEdge(10, i + 1), PM_SUCCESS);
  }

  std::vector<int*> pl = pg.GetPipeline();
  EXPECT_EQ(pl.size(), 6);
  EXPECT_EQ(*pl.at(0), 10);
}

TEST(pmlib_pipeline, one_to_many_2) {
  PipelineGraph pg;
  EXPECT_EQ(pg.AddNode(-1), PM_SUCCESS);
  EXPECT_EQ(pg.AddNode(0), PM_SUCCESS);
  EXPECT_EQ(pg.AddEdge(-1, 0), PM_SUCCESS);

  // -1 -> 0 -> 10 -> 1 -> 100
  //       0 -> 2

  EXPECT_EQ(pg.AddNode(2), PM_SUCCESS);
  EXPECT_EQ(pg.AddEdge(0, 2), PM_SUCCESS);

  EXPECT_EQ(pg.AddNode(10), PM_SUCCESS);
  EXPECT_EQ(pg.AddEdge(0, 10), PM_SUCCESS);

  EXPECT_EQ(pg.AddNode(1), PM_SUCCESS);
  EXPECT_EQ(pg.AddEdge(10, 1), PM_SUCCESS);

  EXPECT_EQ(pg.AddNode(100), PM_SUCCESS);
  EXPECT_EQ(pg.AddEdge(1, 100), PM_SUCCESS);

  std::vector<int*> pl = pg.GetPipeline();
  EXPECT_EQ(pl.size(), 6);
  EXPECT_EQ(*pl.at(0), -1);
  EXPECT_EQ(*pl.at(1), 0);
}

TEST(pmlib_pipeline, short_and_long_path) {
  PipelineGraph pg;
  pg.AddNodes({0, 1}); // start & endif
  pg.AddNodes({10, 11, 12, 13}); // short
  pg.AddEdges({{0, 10}, {10, 11}, {11, 12}, {12, 13}, {13, 1}});
  pg.AddNodes({20, 21, 22, 23, 24, 25, 26}); // long
  pg.AddEdges({{0, 20}, {20, 21}, {21, 22}, {22, 23}, {23, 24}, {24, 25}, {25, 26}, {26, 1}});

  std::vector<int*> pl = pg.GetPipeline();
  EXPECT_EQ(pl.size(), 13);
  EXPECT_EQ(*pl.at(0), 0);
  EXPECT_EQ(*pl.at(pl.size()-1), 1);
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

  std::vector<int*> pl = pg.GetPipeline();
  EXPECT_EQ(pl.size(), 13);
  EXPECT_EQ(*pl.at(12), 1);
}

std::string chains_to_pipelines(std::string str){
  data_string_to_pipeline data;
  string_to_pipeline(str, data);

  PipelineGraph<std::string> pg;
  for(auto node : data.nodes){
    pg.AddNode(node);
  }

  for(auto edge : data.edges){
    pg.AddEdge(edge.first, edge.second);
  }

  std::vector<std::string*> pl = pg.GetPipeline();
  return join_string_ptrs(pl);
}


TEST(pmlib_pipeline, string_to_pipeline) {
  EXPECT_EQ(chains_to_pipelines("  A->B->C,  X->A, D->X    "), "DXABC");
  EXPECT_EQ(chains_to_pipelines("MAPA->DANA(   XXX  )->CAMO-MILE,"), "MAPADANA(   XXX  )CAMO-MILE");
}

#endif




