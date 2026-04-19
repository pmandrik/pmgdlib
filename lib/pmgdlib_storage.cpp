// P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib

#include "pmgdlib_storage.h"

namespace pmgd {
  template<> std::string add_type_prefix<Image>(const std::string & name){return "image:" + name;}
  template<> std::string add_type_prefix<Shader>(const std::string & name){return "shader:" + name;}
  template<> std::string add_type_prefix<Scene>(const std::string & name){return "scene:" + name;}
  template<> std::string add_type_prefix<FrameDrawer>(const std::string & name){return "frame_drawer:" + name;}
  template<> std::string add_type_prefix<TextureDrawer>(const std::string & name){return "texture_drawer:" + name;}
  
  // =======  ====================================================================


  // ======= NdMap ====================================================================
  //! return tree node
  NdMapTree* NdMapTree::Get(const std::string & part_id, bool add){
    auto it = nodes.find(part_id);
    if(it == nodes.end()){
      if(add){
        NdMapTree* answer = new NdMapTree();
        nodes[part_id] = answer;
        return answer;
      }
      return nullptr;
    }
    return it->second;
  }

  NdKey operator + (NdKey ka, NdKey kb){
    NdKey kx;
    for(int i = 0; i < ka.size(); ++i) kx.Add(ka.at(i));
    for(int i = 0; i < kb.size(); ++i) kx.Add(kb.at(i));
    return kx;
  }
};