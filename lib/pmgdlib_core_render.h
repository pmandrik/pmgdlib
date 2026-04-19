// P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib

#ifndef PMGDLIB_CORE_RENDER_HH
#define PMGDLIB_CORE_RENDER_HH 1

#include <list>
#include <memory>
#include <stack>
#include <unordered_map>

#include "pmgdlib_msg.h"
#include "pmgdlib_math.h"

namespace pmgd {
  //================================================ Base classes
  class Texture : public BaseMsg {
    /// base abstract texture class
    public:
    virtual void Bind()   = 0;
    virtual void Unbind() = 0;
    virtual ~Texture() {};
  };

  class Shader : public BaseMsg {
    public:
    virtual int LoadVert(const std::string & text) = 0;
    virtual int LoadFrag(const std::string & text) = 0;
    virtual int CreateProgram() = 0;
    virtual int AddUniform(const std::string name) = 0;
    virtual int GetUniform(const std::string name) = 0;
    virtual void UpdateUniform1f(const int & pos, const float & val) = 0;
    virtual void EnableTexture(const int & pos, const int index) = 0;
    virtual ~Shader(){};
  };

  struct TexTile {
    /// TexTile store the info about position & size of part of image in Texture normalized to 1. space
    TexTile() {}
    TexTile(v2 tp, v2 ts) : tpos(tp), tsize(ts) {}
    v2 tpos, tsize;
  };

  class TexAtlas {
    private:
      std::unordered_map<std::string, TexTile> atlas;
      v2 size;

    public:
      TexTile * Get(const std::string & key);
      void Add(const std::string & key, const v2 & tp, const v2 & ts);
      std::string GenItemKey(int x, int y) const;
      std::string GenItemKey(std::string name, int x, int y) const;
  };

  struct TextureDrawData {
    v2 pos = v2(0,0), size = v2(0.5,0.5);
    v2 tpos = v2(0,0), tsize = v2(1,1);
    float angle = 0;
    bool flip_x = false, flip_y = false;

    TextureDrawData(){}
    TextureDrawData(v2 pos, v2 size){
      this->pos = pos;
      this->size = size;
    }
  };

  //================================================ Drawers
  class Drawable : public BaseMsg {
    public:
    virtual ~Drawable(){};
    virtual void Draw(){};
  };

  class SimpleDrawer : public Drawable {
    public:
    virtual ~SimpleDrawer(){}

    virtual unsigned int Add(TextureDrawData * quad_data) = 0;
    virtual void Remove(const unsigned int & id) = 0;
    virtual void Clean() = 0;
  };

  class FrameDrawer : public Drawable {
  };

  class TextureDrawer : public Drawable {
    public:
    std::string shader_id, texture_id;
    TextureDrawData data;
    Texture *texture;
    Shader *shader;
    TextureDrawer(){};
    virtual ~TextureDrawer(){};
  };

  class ArrayDrawer : public Drawable {
    virtual unsigned int IndexToId(unsigned int quad_index){ return quad_index; };
    virtual unsigned int IdToIndex(unsigned int id){ return id; };
    virtual bool IsFreeIndex(unsigned int quad_index) = 0;
    virtual unsigned int GetDefaultId(){
      if(++last_quad_id >= max_quads_number) last_quad_id = 0;
      return last_quad_id;
    }

    protected:
    bool dirty = false;
    unsigned int last_quad_id = -1, max_quads_number = 0;
    std::stack<unsigned int> free_positions;
    unsigned int FindFreePosition();

    public:
    ArrayDrawer(unsigned int max_quads_number){ this->max_quads_number = max_quads_number; };
    virtual ~ArrayDrawer(){};

    //! add element to the array
    virtual unsigned int Add(TextureDrawData * quad_data) = 0;

    //! update array data full
    virtual void Set(const unsigned int & id, TextureDrawData * quad_data) = 0;

    //! update array data position only
    virtual void SetPos(const unsigned int & id, TextureDrawData * quad_data) = 0;

    //! update array data texture only
    virtual void SetText(const unsigned int & id, TextureDrawData * quad_data) = 0;

    //! change position of element by shift 2d value
    virtual void Move(const unsigned int & id, const v2 & shift) = 0;

    //! remove all elements from array
    virtual void Clean(){};

    //! remove one elements from array
    virtual void Remove(const unsigned int & id) = 0;
  };

  //================================================ Render
  class RenderPipelineItem {
    public:
    bool new_target = true, new_source = true, cleanup_source = true, cleanup_target = true;
    Drawable * target;
    Drawable * source;
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