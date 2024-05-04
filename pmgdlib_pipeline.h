// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_PIPELINE_HH
#define PMGDLIB_PIPELINE_HH 1

#include "pmgdlib_core.h"

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
  class PipelineGraph {
    private:
      class Node {
        public:
          Node(int id, int priority){this->id = id; this->local_priority = local_priority;}
          int id;
          int local_priority;
          int priority = 0;
          std::set<int> sources;
          std::set<int> targets;
      };

      struct CmpNodePtr {
        bool operator()(Node* lhs, Node* rhs) const {
          if(lhs->priority != rhs->priority) return lhs->priority < rhs->priority;
          if(lhs->local_priority != rhs->local_priority) return lhs->local_priority < rhs->local_priority;
          return lhs->id < rhs->id;
        }
      };

      std::unordered_map<int, Node*> nodes;

      bool GetPipelineRec(Node* source, std::set<int>* veto){
        for(auto it = source->sources.begin(); it != source->sources.end(); ++it){
          int node_id = *it;
          std::cout << node_id << " " << source->id << std::endl;
          if(veto->count(node_id)) return false;

          auto find = nodes.find(node_id);
          if(find == nodes.end()) continue;
          auto node = find->second;

          veto->insert(node_id);
          node->priority = source->priority-1;
          if(not GetPipelineRec(node, veto)) return false;
        }
        return true;
      }

    public:
      //! add new node with given id if id not in the graph
      int AddNode(int id, int local_priority = 0){
        if(nodes.count(id)) return PM_ERROR_DUPLICATE;
        nodes[id] = new Node(id, local_priority);
        return PM_SUCCESS;
      }

      //! remove node with given id if id is in the graph
      //! do not touch edges or other nodes
      int RemoveNode(int id){
        auto find = nodes.find(id);
        if(find == nodes.end()) return PM_ERROR_404;
        nodes.erase(find);
        auto node = find->second;
        delete node;
        return PM_SUCCESS;
      }

      //! add edge with direction from source to target
      int AddEdge(int source_id, int target_id){
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

      int RemoveEdge(int source_id, int target_id){
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
      std::vector<int*> GetPipeline() {
        std::vector<int*> answer;

        // get heads - nodes without targets
        std::vector<Node*> heads;
        for(auto it = nodes.begin(); it != nodes.end(); ++it){
          Node* node = it->second;
          if(not node->targets.size())
            heads.push_back(node);
        }
        if(not heads.size()) return answer;

        // set node priority
        std::set<int> veto;
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
          std::cout << it->id << std::endl;
          answer.push_back(&(it->id));
        }

        return answer;
      }
  };
};

#endif







