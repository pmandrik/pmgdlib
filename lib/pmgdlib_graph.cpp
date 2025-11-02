// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#include "pmgdlib_graph.h"
#include "pmgdlib_defs.h"

namespace pmgd {

  // add_strdata_to_pipeline ==========================================================================
  struct parsed_pipeline_str_data {
    //! Pipeline
    //! "sla_back->sla_backbuff->sla_backloop->sla_fbuffer"
    //! expect input as "A->B->C,D->E,E->C"
    std::vector<std::string> nodes;
    std::set<std::string> veto;
    std::vector<std::pair<std::string,std::string>> edges;
  };

  int parse_pipeline_str_part(const std::string &str, parsed_pipeline_str_data &data){
    std::vector<std::string> nodes;
    split_string_strip(str, nodes, "->");
    if(nodes.size() < 2){
      return PM_ERROR_500;
    }

    for(size_t i = 0; i < nodes.size()-1; ++i){
      std::string src = nodes.at(i);
      std::string tgt = nodes.at(i+1);
      if(not data.veto.count(src)) {
        data.nodes.push_back(src);
        data.veto.insert(src);
      }
      if(not data.veto.count(tgt)) {
        data.nodes.push_back(tgt);
        data.veto.insert(tgt);
      }
      data.edges.push_back(std::make_pair(src, tgt));
    }

    return PM_SUCCESS;
  }

  int parse_pipeline_str(const std::string &str, parsed_pipeline_str_data &data){
    std::vector<std::string> chains;
    split_string_strip(str, chains, ",");

    for(auto chain: chains){
      int ret = parse_pipeline_str_part(chain, data);
      if(ret != PM_SUCCESS)
          return ret;
    }
    return PM_SUCCESS;
  }

  // TODO:
  // image I1 -> draw into FB1
  // image I2 -> draw into FB2

  // 1) FB1 draw into FB3
  // 2) use FB1 + FB2 to draw into FB3

  // 1) I1->FB1, I2->FB2, FB1->FB3
  // 2) I1->FB1, I2->FB2, FB12(FB1, FB2)->FB3
  int add_strdata_to_pipeline(const std::string &str, PipelineGraph<std::string> &pg){
    parsed_pipeline_str_data data;
    int ret = parse_pipeline_str(str, data);
    if(ret != PM_SUCCESS) return ret;

    int lp = 0;
    for(auto node : data.nodes){
      pg.AddNode(node, lp);
      lp++;
    }
  
    for(auto edge : data.edges){
      pg.AddEdge(edge.first, edge.second);
    }
  
    return PM_SUCCESS;
  }
};