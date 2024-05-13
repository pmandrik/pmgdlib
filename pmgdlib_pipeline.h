// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_PIPELINE_HH
#define PMGDLIB_PIPELINE_HH 1

#include "pmgdlib_core.h"
#include "pmgdlib_string.h"

namespace pmgd {

  //! PipelineGraph:
  //!   Node = [int id],
  //!   Edge = [source node, target node]
  //! Targets:
  //!   GetPipeline() build vector with elements where all targets are after sources or raise if it is not possible (detect loops).
  //!   Node can have multiple sources and multiple targets.
  //!   Sources of one node are sorted by priority.
  //!   Graph maybe disconnected, then any order of separated parts is valid.
  //! Not RT
  template <typename NODE_ID_TYPE = int>
  class PipelineGraph {
    private:
      class Node {
        public:
          Node(NODE_ID_TYPE id, int priority){this->id = id; this->local_priority = local_priority;}
          NODE_ID_TYPE id;
          int local_priority;
          int priority = 0;
          std::set<NODE_ID_TYPE> sources;
          std::set<NODE_ID_TYPE> targets;
      };

      struct CmpNodePtr {
        bool operator()(Node* lhs, Node* rhs) const {
          if(lhs->priority != rhs->priority) return lhs->priority < rhs->priority;
          if(lhs->local_priority != rhs->local_priority) return lhs->local_priority < rhs->local_priority;
          return lhs->id < rhs->id;
        }
      };

      std::unordered_map<NODE_ID_TYPE, Node*> nodes;

      bool GetPipelineRec(Node* head, std::set<NODE_ID_TYPE>* veto){
        int new_priority = head->priority-1;
        for(auto it = head->sources.begin(); it != head->sources.end(); ++it){
          NODE_ID_TYPE node_id = *it;
          // std::cout << "node, head = " << node_id << " " << head->id << std::endl;
          if(veto->count(node_id)) return false;

          auto find = nodes.find(node_id);
          if(find == nodes.end()) continue;
          auto node = find->second;

          if(node->priority <= new_priority) continue;
          veto->insert(node_id);
          node->priority = head->priority-1;
          if(not GetPipelineRec(node, veto)) return false;
          veto->erase(node_id);
        }
        return true;
      }

    public:
      //! add new node with given id if id not in the graph
      int AddNode(NODE_ID_TYPE id, int local_priority = 0){
        if(nodes.count(id)) return PM_ERROR_DUPLICATE;
        nodes[id] = new Node(id, local_priority);
        return PM_SUCCESS;
      }

      int AddNodes(std::vector<NODE_ID_TYPE> ids){
        for(int ret, i = 0; i < (int)ids.size(); ++i){
          if((ret = AddNode(ids.at(i), 0)) != PM_SUCCESS)
            return ret;
        }
        return PM_SUCCESS;
      }

      //! remove node with given id if id is in the graph
      //! do not touch edges or other nodes
      int RemoveNode(NODE_ID_TYPE id){
        auto find = nodes.find(id);
        if(find == nodes.end()) return PM_ERROR_404;
        nodes.erase(find);
        auto node = find->second;
        delete node;
        return PM_SUCCESS;
      }

      //! add edge with direction from source to target
      int AddEdge(NODE_ID_TYPE source_id, NODE_ID_TYPE target_id){
        auto find_target = nodes.find(target_id);
        if(find_target == nodes.end()) return PM_ERROR_404;

        auto find_source = nodes.find(source_id);
        if(find_source == nodes.end()) return PM_ERROR_404;

        Node* node_target = find_target->second;
        node_target->sources.insert(source_id);

        Node* node_source = find_source->second;
        node_source->targets.insert(target_id);

        return PM_SUCCESS;
      }

//       int AddEdges(std::vector<int> edges){
//         return AddEdge(edges.at(0), edges.at(1));
//       }
//
//       int AddEdges(std::vector<int> edge, std::vector<int> edges...){
//         if(int ret; (ret = AddEdge(edge.at(0), edge.at(1))) != PM_SUCCESS) return ret;
//         return AddEdges(edges);
//       }

      int AddEdges(std::vector<std::vector<NODE_ID_TYPE>> edges){
        for(int ret, i = 0; i < (int)edges.size(); ++i){
          if((ret = AddEdge(edges.at(i).at(0), edges.at(i).at(1))) != PM_SUCCESS)
            return ret;
        }
        return PM_SUCCESS;
      }

      int RemoveEdge(NODE_ID_TYPE source_id, NODE_ID_TYPE target_id){
        auto find_target = nodes.find(target_id);
        if(find_target != nodes.end()) {
          Node* node_target = find_target->second;
          if(not node_target->sources.erase(source_id)) return PM_ERROR_404;
        }
        auto find_source = nodes.find(source_id);
        if(find_source != nodes.end()) {
          Node* node_source = find_source->second;
          if(not node_source->targets.erase(target_id)) return PM_ERROR_404;
        }
        return PM_SUCCESS;
      }

      //! build vector with elements where all targets are after sources
      std::vector<NODE_ID_TYPE*> GetPipeline(){
        std::vector<NODE_ID_TYPE*> answer;

        // get heads - nodes without targets
        std::vector<Node*> heads;
        for(auto it = nodes.begin(); it != nodes.end(); ++it){
          Node* node = it->second;
          if(not node->targets.size())
            heads.push_back(node);
        }
        if(not heads.size()) return answer;

        // set node priority
        std::set<NODE_ID_TYPE> veto;
        for(auto head : heads){
          veto.insert(head->id);
          if(not GetPipelineRec(head, &veto)) return answer;
          veto.clear();
        }

        // sort nodes using priority
        std::set<Node*, CmpNodePtr> all_nodes_sorted;
        for(auto it = nodes.begin(); it != nodes.end(); ++it){
          Node* node = it->second;
          all_nodes_sorted.insert(node);
        }

        // fill answer with data
        for(auto it: all_nodes_sorted){
          // std::cout << it->id << " pr " << it->priority << std::endl;
          answer.push_back(&(it->id));
        }

        return answer;
      }
  };


  //! Pipeline
  //! "sla_back->sla_backbuff->sla_backloop->sla_fbuffer"
  /// expect input as "A->B->C,D->E,E->C"
  //       /// at first split into "A->B->C", "D->E", "E->C"
  //       std::vector<std::string> value_cpp_parts;
  //       pm::split_string_strip(relation, value_cpp_parts, ",");
  //       if( not value_cpp_parts.size() ){
  //         msg_err(__PFN__, "skip empty relation");
  //         return;
  //       } MSG_DEBUG(__PFN__, "find relation with N parts = ", value_cpp_parts.size());
  //
  //       /// then split "A->B->C" split into "A", "B", "C"
  //       /// and add relations as AddRelation("A","B"), AddRelation("B","C")
  //       for(auto value_cpp_part : value_cpp_parts){
  //         std::vector<std::string> conn_parts;
  //         pm::split_string_strip(value_cpp_part, conn_parts, "->");
  //         if( conn_parts.size() < 2 ){
  //           msg_err(__PFN__, "skip relation without \"->\" separated parts");
  //           continue;
  //         } MSG_DEBUG(__PFN__, "find relation with N in connection = ", conn_parts.size());
  //
  //         for(int i = 0; i < conn_parts.size()-1; ++i){
  //           string src = conn_parts[i];
  //           string tgt = conn_parts[i+1];
  //           AddRelation( src , tgt );
  //         }
  //       }

  struct data_string_to_pipeline {
    std::set<std::string> nodes;
    std::vector<std::pair<std::string,std::string>> edges;
  };

  int chain_to_pipeline(const std::string &str, data_string_to_pipeline &data){
    std::vector<std::string> nodes;
    split_string_strip(str, nodes, "->");
    if(nodes.size() < 2){
      return PM_ERROR_500;
    }

    for(size_t i = 0; i < nodes.size()-1; ++i){
      std::string src = nodes[i];
      std::string tgt = nodes[i+1];

      data.nodes.insert(src);
      data.nodes.insert(tgt);
      data.edges.push_back(std::make_pair(src, tgt));
    }

    return PM_SUCCESS;
  }

  int string_to_pipeline(const std::string &str, data_string_to_pipeline &data){
    std::vector<std::string> chains;
    split_string_strip(str, chains, ",");

    for(auto chain: chains){
      int ret = chain_to_pipeline(chain, data);
      if(ret != PM_SUCCESS)
          return ret;
    }
    return PM_SUCCESS;
  }
};

#endif







