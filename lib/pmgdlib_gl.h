// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_GL_HH
#define PMGDLIB_GL_HH 1

#include "pmgdlib_defs.h"
#include "pmgdlib_math.h"
#include "pmgdlib_image.h"
#include "pmgdlib_core.h"
#include "pmgdlib_config.h"

#include <GLES3/gl3.h>

namespace pmgd {

  /// Dependencies:
  /// glBegin
  /// GL_QUADS
  /// glVertex3f
  /// glGenTextures
  /// glEnable
  /// glTexCoord2f
  /// glBindTexture
  /// glGenerateMipmap
  /// glTexParameteri
  /// glGetError

  std::string gl_get_errors_msg(){
    GLenum code;
    std::string answer;
    int max_depths = 100;
    while((code = glGetError()) != GL_NO_ERROR){
      answer += "glGetError():" + std::to_string(code) + "\n";
      if(max_depths-- < 0){
        answer += "glGetError(): max depths reached\n";
      }
    }
    return answer;
  }

  void GLAPIENTRY
  MessageCallback( GLenum source,
                  GLenum type,
                  GLuint id,
                  GLenum severity,
                  GLsizei length,
                  const GLchar* message,
                  const void* userParam )
  {
    /// https://www.khronos.org/opengl/wiki/Debug_Output
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
              type, severity, message );

    // During init, enable debug output
    // glEnable              ( GL_DEBUG_OUTPUT );
    // glDebugMessageCallback( MessageCallback, 0 );
  }


  // ======= simple draw functions ====================================================================
  void draw_quad(const v2 & pos, v2 size, const float & angle){
    v2 perp = v2(-size.x, size.y);
    size = size.Rotated(angle);
    perp = perp.Rotated(angle);
    glBegin(GL_TRIANGLES);
    glVertex3f(pos.x - size.x, pos.y + size.y, pos.z);
	  glVertex3f(pos.x - perp.x, pos.y + perp.y, pos.z);
	  glVertex3f(pos.x + size.x, pos.y - size.y, pos.z);
    glVertex3f(pos.x + size.x, pos.y - size.y, pos.z);
	  glVertex3f(pos.x + perp.x, pos.y - perp.y, pos.z);
    glVertex3f(pos.x - size.x, pos.y + size.y, pos.z);
	  glEnd();
  }

  void draw_textured_quad(const v2 & pos, v2 size, const v2 & tpos, const v2 & tsize, const float & angle, const bool & flip_x, const bool & flip_y){
    if(flip_x) size.x *= -1;
    if(flip_y) size.y *= -1;

    v2 perp = v2(-size.x, size.y);
    size = size.Rotated(angle);
    perp = perp.Rotated(angle);

    glBegin(GL_TRIANGLES);
    glTexCoord2f(tpos.x, 	tpos.y);                     glVertex3f(pos.x - size.x, pos.y + size.y, pos.z);
	  glTexCoord2f(tpos.x + tsize.x, 	tpos.y);           glVertex3f(pos.x - perp.x, pos.y + perp.y, pos.z);
	  glTexCoord2f(tpos.x + tsize.x, 	tpos.y + tsize.y); glVertex3f(pos.x + size.x, pos.y - size.y, pos.z);
    glTexCoord2f(tpos.x + tsize.x, 	tpos.y + tsize.y); glVertex3f(pos.x + size.x, pos.y - size.y, pos.z);
	  glTexCoord2f(tpos.x, 	tpos.y + tsize.y);           glVertex3f(pos.x + perp.x, pos.y - perp.y, pos.z);
    glTexCoord2f(tpos.x, 	tpos.y);                     glVertex3f(pos.x - size.x, pos.y + size.y, pos.z);
	  glEnd();
  }

  // ======= draw array of quads ====================================================================
  void fill_verices_array(float * vertices, const int & index, v2 & pos, v2 size, const float & angle, int stride=3){
    v2 perp = v2(-size.x, size.y);
    size = size.Rotated(angle);
    perp = perp.Rotated(angle);

    vertices[index]   = pos.x - size.x;
    vertices[index+1] = pos.y + size.y;
    vertices[index+2] = pos.z;

    vertices[index  +stride] = pos.x - perp.x;
    vertices[index+1+stride] = pos.y + perp.y;
    vertices[index+2+stride] = pos.z;

    vertices[index  +2*stride] = pos.x + size.x;
    vertices[index+1+2*stride] = pos.y - size.y;
    vertices[index+2+2*stride] = pos.z;

    vertices[index  +3*stride] = pos.x + perp.x;
    vertices[index+1+3*stride] = pos.y - perp.y;
    vertices[index+2+3*stride] = pos.z;
  }

  void fill_texture_array(float * vertices, const int & index, const v2 & tpos, const v2 & tsize, int stride=2){
    vertices[index]   = tpos.x;
    vertices[index+1] = tpos.y;

    vertices[index  +stride] = tpos.x + tsize.x;
    vertices[index+1+stride] = tpos.y;

    vertices[index  +2*stride] = tpos.x + tsize.x;
    vertices[index+1+2*stride] = tpos.y + tsize.y;

    vertices[index  +3*stride] = tpos.x;
    vertices[index+1+3*stride] = tpos.y + tsize.y;
  }

  int* fill_indexes_array(int n_quads){
    int n_indexes = n_quads*6;
    int *indices = new int[n_indexes];

    for(int i = 0; i < n_indexes; i++){
      int vi = i / 6;
      int qi = i % 6;
      indices[i] = vi*4 + qi - (qi>2) - 4*(qi==5);
      // printf("%d,", indices[i]);
      // if(qi == 5) printf("\n");
    }

    return indices;
  }

  void draw_quad_array(float *vertices, int n_quads){
    /// slow, for tests
    int *indices = fill_indexes_array(n_quads);

    glEnableClientState(GL_VERTEX_ARRAY);
    // TODO deprecate glVertexPointer
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawElements(GL_TRIANGLES, n_quads*6, GL_UNSIGNED_INT, indices);
    glDisableClientState(GL_VERTEX_ARRAY);

    delete[] indices;
  }

  void draw_textured_elements(int n_vertexes, float *data, int n_indexes, int *indexes){
    /// slow, for testing
    GLuint VAOId;
    glGenVertexArrays(1, &VAOId);
    glBindVertexArray(VAOId);

    GLuint VBOId;
    glGenBuffers(1, &VBOId);

    // fill data
    glBindBuffer(GL_ARRAY_BUFFER, VBOId);
    glBufferData(GL_ARRAY_BUFFER, n_vertexes*5*sizeof(GLfloat), data, GL_STATIC_DRAW);

    // specify position attribute
    // index - 0
    // size - 3 v3 pos(x, y, z)
    // data type - GL_FLOAT
    // normalized - false
    // array groudp size = 3 pos + 2 texts
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // specify texture coordinate
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_indexes*sizeof(GLint), indexes, GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLES, n_indexes, GL_UNSIGNED_INT, (GLvoid*)0);

    // tear down
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &VBOId);
    glDeleteVertexArrays(1, &VAOId);

    glDeleteBuffers(1, &elementbuffer);
    glDeleteVertexArrays(1, &elementbuffer);
  }

  // ======= texture ====================================================================
  GLuint image_format_to_gl(int format){
    if(format == image_format::RGBA) return GL_RGBA;
    return image_format::UNDEFINED;
  };

  GLuint image_type_to_gl(int type){
    if(type == image_type::UNSIGNED_CHAR) return GL_UNSIGNED_BYTE;
    if(type == image_type::UNSIGNED_INT) return GL_UNSIGNED_INT;
    if(type == image_type::FLOAT) return GL_FLOAT;
    return image_type::UNDEFINED;
  };

  class TextureGl : public Texture {
    /// thread unsafe
    private :
      GLuint id = 0, type = 0, format = 0, internalformat = 0;
      bool is_binded = false;

    public:
      ~TextureGl() {
        if(id)
          glDeleteTextures(1, &id);
      }

      GLuint GetId() const { return id; }

      TextureGl(std::shared_ptr<Image> img){
        format = image_format_to_gl(img->format);
        type   = image_type_to_gl(img->type);
        internalformat = format;
        if(format == (GLuint)image_format::UNDEFINED){
          msg_warning("invalid image format", img->format);
          return;
        }
        if(type == (GLuint)image_type::UNDEFINED){
          msg_warning("invalid image type", img->type);
          return;
        }

        glGenTextures(1, &id);
        if(not id){
          msg_warning("glGenTextures failed");
          msg_warning(gl_get_errors_msg());
          return;
        }

        Bind();
        glEnable(GL_TEXTURE_2D); // TODO use or not?
        // glTexStorage2D(GL_TEXTURE_2D, MIP_LEVELS_FIXME, GL_RGBA, w, h);
        // void glTexImage2D(target, level, internalFormat, width, height, border, format, type, data);
        // msg( w, h, type, format, GL_FLOAT, GL_RGBA );
        // for(int i = 0; i < 10; i++) msg( i, (int)((unsigned char*) (image->data))[ i ] );

        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, img->w, img->h, 0, format, type, img->data);
        // void glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, const void *data);
        // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, image->format, image->type, image->data);

        // TODO use or not?
        // glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        std::string any_errors = gl_get_errors_msg();
        if(any_errors.size()){
          msg_warning("GL texture setup failed");
          msg_warning(gl_get_errors_msg());
          return;
        }

        Unbind();
      }

    public:
      virtual void Bind()  { if(    is_binded) return; glBindTexture(GL_TEXTURE_2D, id); is_binded = true;  }
      virtual void Unbind(){ if(not is_binded) return; glBindTexture(GL_TEXTURE_2D,  0); is_binded = false; }

      virtual void Draw(const TextureDrawData & data){
        Bind();
        glEnable(GL_TEXTURE_2D);
        draw_textured_quad(data.pos, data.size, data.tpos, data.tsize, data.angle, data.flip_x, data.flip_y);
        Unbind();
      }
  };

  // ======= texture ====================================================================
  class ShaderGl : public Shader {
    GLuint shader_vert = 0, shader_frag = 0;
    GLuint program_id = 0;
    std::unordered_map<std::string, int> uniforms;

    public:
    GLuint GetProgramId() const { return program_id; }

    int Load(GLenum type, const std::string & text){
      msg_debug("load shader ...");
      msg_verbose(text);
      if(type != GL_VERTEX_SHADER and type != GL_FRAGMENT_SHADER){
        msg_warning("invalid shader type argument", type, " valid=[", GL_VERTEX_SHADER, ",", GL_FRAGMENT_SHADER, "]");
        return PM_ERROR_INCORRECT_ARGUMENTS;
      }

      GLuint id = glCreateShader(type);

      if(not id){
        msg_warning("glCreateShader failed");
        msg_warning(gl_get_errors_msg());
        return PM_ERROR_GL;
      }
      msg_debug("glCreateShader ok", id);

      const char * ctext = text.c_str();
      glShaderSource(id, 1, &ctext, NULL);

      GLint status;
      glCompileShader(id);
      glGetShaderiv(id, GL_COMPILE_STATUS, &status);

      if(status == GL_FALSE){
        msg_warning("Shader compile error");
        GLint lenght;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
        GLchar * log = new GLchar[lenght];
        glGetShaderInfoLog(id, lenght, NULL, log);
        msg_warning(log);
        delete [] log;
        return PM_ERROR_GL;
      }
      msg_debug("glCompileShader ok", id);

      if(type == GL_VERTEX_SHADER)   {
        if(shader_vert) msg_warning("override existed GL_VERTEX_SHADER shader");
        shader_vert = id;
      }
      if(type == GL_FRAGMENT_SHADER) {
        if(shader_frag) msg_warning("override existed GL_FRAGMENT_SHADER shader");
        shader_frag  = id;
      }

      msg_debug("load shader ... ok");
      return PM_SUCCESS;
    }

    ~ShaderGl() override {
      msg_debug("shader destructor call");
      // TODO
    }
    virtual int LoadVert(const std::string & text){ return Load(GL_VERTEX_SHADER, text); };
    virtual int LoadFrag(const std::string & text){ return Load(GL_FRAGMENT_SHADER, text); };

    int CreateProgram(){
      msg_debug("load shader programm ...");
      if(not shader_vert and not shader_frag){
        msg_warning("no loaded shaders");
        return PM_ERROR_CLASS_ATTRIBUTES;
      }

      program_id = glCreateProgram();
      if(not program_id){
        msg_warning("glCreateProgram failed");
        msg_warning(gl_get_errors_msg());
        return PM_ERROR_GL;
      }
      if( shader_vert ) glAttachShader(program_id, shader_vert);
      if( shader_frag ) glAttachShader(program_id, shader_frag);

      GLint status;
      glLinkProgram(program_id);
      glGetProgramiv(program_id, GL_LINK_STATUS, &status);
      if(status == GL_FALSE){
        msg_warning("program link error");
        GLint lenght;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &lenght);
        GLchar * log = new GLchar[lenght];
        glGetShaderInfoLog(program_id, lenght, NULL, log);
        msg_warning(log);
        delete [] log;
      }
      msg_debug("program link ok");

      // https://stackoverflow.com/questions/8829288/may-i-call-gldeleteshader-after-calling-gllinkprogram
      glDeleteShader( shader_vert );
      glDeleteShader( shader_frag );

      // TODO Uniforms
      int n_uniforms;
      glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &n_uniforms); // glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count);
      // MSG_DEBUG("Shader.CreateProgram():", shader_name," shaders program has", program_id);

      msg_debug("load shader programm ... ok");
      return PM_SUCCESS;
    }

    virtual void Bind(){   glUseProgram( program_id ); };
    virtual void Unbind(){ glUseProgram( 0 ); };

    virtual int AddUniform(const std::string name){
      if(uniforms.find(name) != uniforms.end()){
        msg_warning("uniform \"" + name + "\" already in map, skip");
        return PM_ERROR_INCORRECT_ARGUMENTS;
      }

      if(not program_id){
        msg_warning("program_id 0, call CreateProgram() first");
        return PM_ERROR_GL;
      }
      const GLchar * name_cstr = (const GLchar *) name.c_str();
      Bind();
      GLint position = glGetUniformLocation(program_id, name_cstr);
      Unbind();
      if(position == -1){
        msg_warning("glGetUniformLocation() return -1 for variable name of the uniform variable \""+name+"\"");
        return PM_ERROR_GL;
      }
      msg_debug("find uniform \"" + name + "\" at position", position);
      uniforms[name] = position;
      return PM_SUCCESS;
    }

    virtual int GetUniform(const std::string name){
      std::unordered_map <std::string, int>::iterator it = uniforms.find(name);
      if(it == uniforms.end()){
        msg_warning("can't find uniform variable with name\""+name+"\", add it first with AddUniform()");
        return -1;
      }
      return it->second;
    }

    virtual void UpdateUniform1f(const int & pos, const float & val){
      glUniform1f( (GLint) pos, val );
    }

    virtual void EnableTexture(const int & pos, const int index){
        if( index == 0 ){
          glActiveTexture(GL_TEXTURE0);
          glUniform1i((GLint) pos, 0);
        } else  if( index == 1 ){
          glActiveTexture(GL_TEXTURE1);
          glUniform1i((GLint) pos, 1);
        } else if( index == 2 ){
          glActiveTexture(GL_TEXTURE2);
          glUniform1i((GLint) pos, 2);
        } else {
          msg_warning("index (", index, ") must be in range [0, 2]");
        }
    }

    virtual int AddUniformsAuto(){
      msg_debug("add uniforms auto ...");
      
      GLint numActiveAttribs = 0;
      GLint numActiveUniforms = 0;
      glGetProgramiv(program_id, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
      glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

      std::vector<GLchar> nameData(256);
      for(int unif = 0; unif < numActiveUniforms; ++unif) {
        GLint arraySize = 0;
        GLenum type = 0;
        GLsizei actualLength = 0;
        glGetActiveUniform(program_id, unif, nameData.size(), &actualLength, &arraySize, &type, &nameData[0]);
        std::string name((char*)&nameData[0], actualLength);

        int status = AddUniform(name);
        if(not status) return status;
      }

      msg_debug("add uniforms auto ... ok");
      return PM_SUCCESS;
    }
  };

  void draw_textured_quad(TextureGl & text, const TextureDrawData & data){
    text.Bind();
    glEnable(GL_TEXTURE_2D);
    draw_textured_quad(data.pos, data.size, data.tpos, data.tsize, data.angle, data.flip_x, data.flip_y);
    text.Unbind();
  };

  class QuadsArrayGl : public QuadsArray {
    float* data;
    unsigned int array_size, array_size10;
    int* indexes;
    unsigned int n_indexes;

    static const int quad_vertexes = 4; // 4 vertexes per quad :D
    static const int vertex_attributes = 5; // 5 attributes per vertex :D
    static const int quad_aray_size = quad_vertexes * vertex_attributes;

    GLuint vertex_array_id = 0, data_buffer_id = 0, index_buffer_id = 0;

    virtual unsigned int IndexToId(unsigned int quad_index){ return quad_index*quad_aray_size; };
    virtual unsigned int IdToIndex(unsigned int id){ return id/quad_aray_size; };
    virtual bool IsFreeIndex(unsigned int quad_index){ return data[quad_index*quad_aray_size+2] <= sys::PERSPECTIVE_EDGE;
    };

    public:
    QuadsArrayGl(unsigned int max_quads_number) : QuadsArray(max_quads_number) {
      msg_debug("constructor call");
      array_size = quad_aray_size * max_quads_number;
      array_size10 = array_size/10;
      if(array_size10 == 0) array_size10 = array_size;
      data = new float[array_size];
      Clean();

      n_indexes = 6 * max_quads_number;
      indexes = fill_indexes_array(max_quads_number);

      msg_debug("generate buffers");
      glGenVertexArrays(1, &vertex_array_id);
      if(not vertex_array_id){
        msg_warning("glGenVertexArrays failed");
        msg_warning(gl_get_errors_msg());
        return;
      }
      glBindVertexArray(vertex_array_id);

      glGenBuffers(1, &data_buffer_id);
      if(not data_buffer_id){
        msg_warning("glGenBuffers data buffer failed");
        msg_warning(gl_get_errors_msg());
        return;
      }

      glGenBuffers(1, &index_buffer_id);
      if(not index_buffer_id){
        msg_warning("glGenBuffers index buffer failed");
        msg_warning(gl_get_errors_msg());
        return;
      }

      msg_debug("setup buffers data");
      glBindBuffer(GL_ARRAY_BUFFER, data_buffer_id);
      glBufferData(GL_ARRAY_BUFFER, array_size*sizeof(GLfloat), data, GL_DYNAMIC_DRAW);

      // specify position attribute
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_attributes * sizeof(GLfloat), (GLvoid*)0);
      glEnableVertexAttribArray(0);

      // specify texture coordinate
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, vertex_attributes * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
      glEnableVertexAttribArray(1);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_indexes*sizeof(GLint), indexes, GL_STATIC_DRAW);

      // tear down
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
    }

    virtual ~QuadsArrayGl(){
      msg_debug("destructor call");
      delete[] data;
      delete[] indexes;

      if(data_buffer_id){
        glDeleteBuffers(1, &data_buffer_id);
        glDeleteVertexArrays(1, &data_buffer_id);
      }

      if(index_buffer_id){
        glDeleteBuffers(1, &index_buffer_id);
        glDeleteVertexArrays(1, &index_buffer_id);
      }
    }

    virtual unsigned int Add(TextureDrawData * quad_data) {
      unsigned int free_index = FindFreePosition();
      unsigned int id = IndexToId(free_index);
      Set(id, quad_data);
      return id;
    }

    virtual void Set(const unsigned int & id, TextureDrawData * quad_data) {
      const v2 & pos = quad_data->pos; v2 size = quad_data->size;
      const v2 & tpos = quad_data->tpos; const v2 & tsize = quad_data->tsize;
      const float & angle = quad_data->angle;
      if(quad_data->flip_x) size.x *= -1;
      if(quad_data->flip_y) size.y *= -1;

      v2 perp = v2(-size.x, size.y);
      if(angle){
        size = size.Rotated(angle);
        perp = perp.Rotated(angle);
      }

      data[id+0]  = pos.x - size.x; data[id+3]  = tpos.x;
      data[id+1]  = pos.y + size.y; data[id+4]  = tpos.y;
      data[id+2]  = pos.z;

      data[id+5]  = pos.x - perp.x; data[id+8]  = tpos.x + tsize.x;
      data[id+6]  = pos.y + perp.y; data[id+9]  = tpos.y;
      data[id+7]  = pos.z;

      data[id+10] = pos.x + size.x; data[id+13] = tpos.x + tsize.x;
      data[id+11] = pos.y - size.y; data[id+14] = tpos.y + tsize.y;
      data[id+12] = pos.z;

      data[id+15] = pos.x + perp.x; data[id+18] = tpos.x;
      data[id+16] = pos.y - perp.y; data[id+19] = tpos.y + tsize.y;
      data[id+17] = pos.z;
      dirty = true;
    };

    virtual void SetPos(const unsigned int & id, TextureDrawData * quad_data){
      const v2 & pos = quad_data->pos; v2 size = quad_data->size;
      data[id+0]  = pos.x - size.x;
      data[id+1]  = pos.y + size.y;

      data[id+5]  = pos.x + size.x;
      data[id+6]  = pos.y + size.y;

      data[id+10] = pos.x + size.x;
      data[id+11] = pos.y - size.y;

      data[id+15] = pos.x - size.x;
      data[id+16] = pos.y - size.y;
      dirty = true;
    };

    virtual void SetText(const unsigned int & id, TextureDrawData * quad_data) {
      const v2 & tpos = quad_data->tpos; const v2 & tsize = quad_data->tsize;
      data[id+3]  = tpos.x;
      data[id+4]  = tpos.y;

      data[id+8]  = tpos.x + tsize.x;
      data[id+9]  = tpos.y;

      data[id+13] = tpos.x + tsize.x;
      data[id+14] = tpos.y + tsize.y;

      data[id+18] = tpos.x;
      data[id+19] = tpos.y + tsize.y;
      dirty = true;
    }

    virtual void Move(const unsigned int & id, const v2 & shift) {
      data[id+0]  += shift.x;
      data[id+1]  += shift.y;

      data[id+5]  += shift.x;
      data[id+6]  += shift.y;

      data[id+10] += shift.x;
      data[id+11] += shift.y;

      data[id+15] += shift.x;
      data[id+16] += shift.y;
      dirty = true;
    }

    virtual void Clean(){
      for(unsigned int id = 0; id < array_size; id+=quad_aray_size){
        data[id+2]  = sys::PERSPECTIVE_EDGE;
        data[id+7]  = sys::PERSPECTIVE_EDGE;
        data[id+12] = sys::PERSPECTIVE_EDGE;
        data[id+17] = sys::PERSPECTIVE_EDGE;
      }
      last_quad_id = -1;
      while(free_positions.size()) free_positions.pop();
      dirty = true;
    }

    virtual void Remove(const unsigned int & id){
      data[id+2]  = sys::PERSPECTIVE_EDGE;
      data[id+7]  = sys::PERSPECTIVE_EDGE;
      data[id+12] = sys::PERSPECTIVE_EDGE;
      data[id+17] = sys::PERSPECTIVE_EDGE;
      if(free_positions.size() < array_size10) free_positions.push(IdToIndex(id));
      dirty = true;
    }

    void Draw(){
      //draw_textured_elements(max_quads_number*4, data, max_quads_number*6, indexes);
      glBindVertexArray(vertex_array_id);
      glBindBuffer(GL_ARRAY_BUFFER, data_buffer_id);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
      if(dirty){
        /// TODO update only part
        glBindBuffer(GL_ARRAY_BUFFER, data_buffer_id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, array_size*sizeof(GLfloat), data);
        glDrawElements(GL_TRIANGLES, n_indexes, GL_UNSIGNED_INT, (GLvoid*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
      } else {
        glDrawElements(GL_TRIANGLES, n_indexes, GL_UNSIGNED_INT, (GLvoid*)0);
      }
      glBindVertexArray(0);
      dirty = false;
    }
  };

  class FrameBufferGl : public FrameBuffer {
    GLuint fbo_id, texture_id, depth_id;

    /// OpenGL frame buffer implementation
    public :
      FrameBufferGl(int size_x, int size_y) : FrameBuffer(size_x, size_y) {
        msg_debug("create frame buffer ...");
        // Texture
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &texture_id);
        if(not texture_id){
          msg_warning("glGenTextures failed");
          msg_warning(gl_get_errors_msg());
          return;
        }
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)size_x, (GLsizei)size_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Depth buffer
        glGenRenderbuffers(1, &depth_id);
        if(not depth_id){
          msg_warning("glGenRenderbuffers failed");
          msg_warning(gl_get_errors_msg());
          return;
        }
        glBindRenderbuffer(GL_RENDERBUFFER, depth_id);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, (GLsizei)size_x, (GLsizei)size_y);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // Framebuffer to link everything together
        glGenFramebuffers(1, &fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_id);

        // check
        GLenum status;
        if((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
          msg_err("glCheckFramebufferStatus invalid status", status);
          return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        msg_debug("fcreate frame buffer ... ok", fbo_id, texture_id, depth_id);
      }

      virtual void Target(){
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
        glViewport( 0, 0, size_x, size_y );
      }
      virtual void Untarget(){ glBindFramebuffer(GL_FRAMEBUFFER, 0); }

      virtual void Clear(){
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
        glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
      }

      virtual void BindTexture(){
        // glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, texture_id);
      }

      virtual void UnbindTexture(){
        // glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, 0);
      }

      /*
       * void DrawOnWindow(const bool & mirror_x=false, const bool & mirror_y=false){
        if(mirror_x and mirror_y) Draw(sys::WW2, sys::WH2, -sys::WW2, -sys::WH2);
        else if(mirror_x) Draw(sys::WW2, -sys::WH2, -sys::WW2, sys::WH2);
        else if(mirror_y) Draw(-sys::WW2, sys::WH2, sys::WW2, -sys::WH2);
        else Draw(-sys::WW2, -sys::WH2, sys::WW2, sys::WH2);
      }
      */
  };

  void draw_fb_quad(FrameBufferGl & fb, const TextureDrawData & data){
    fb.BindTexture();
    glEnable(GL_TEXTURE_2D);
    draw_textured_quad(data.pos, data.size, data.tpos, data.tsize, data.angle, data.flip_x, data.flip_y);
    fb.UnbindTexture();
  };

  void draw_dfb_quad(DoubleFrameBuffer & dfb, const TextureDrawData & data){
    dfb.BindTexture();
    glEnable(GL_TEXTURE_2D);
    draw_textured_quad(data.pos, data.size, data.tpos, data.tsize, data.angle, data.flip_x, data.flip_y);
    dfb.UnbindTexture();
  };

  void draw_dfb_back_quad(DoubleFrameBuffer & dfb, const TextureDrawData & data){
    glEnable(GL_TEXTURE_2D);
    draw_textured_quad(data.pos, data.size, data.tpos, data.tsize, data.angle, data.flip_x, data.flip_y);
  };

/*
['glMatrixMode', 'glLoadMatrixf', 'glLoadIdentity', 'glOrtho', '', 'glGetFloatv', 'glRotatef', 'glTranslatef', 'gluLookAt', 'gle){', '', 'gle);', '', '', 'gle,', 'glTexCoord2f', 'glEnableClientState', 'glVertexPointer', 'glTexCoordPointer', 'glEnable', 'glDrawArrays', 'gle=0,', 'glActiveTexture', 'glGenTextures', 'glBindTexture', 'glTexParameteri', 'glTexImage2D', 'glGenRenderbuffers', 'glBindRenderbuffer', 'glRenderbufferStorage', 'glGenFramebuffers', 'glBindFramebuffer', 'glFramebufferTexture2D', 'glFramebufferRenderbuffer', 'glCheckFramebufferStatus', 'glCheckFramebufferStatus"', 'glClearColor', '', '', 'gl_check_error', 'glGetError', 'glTexImage2D_err', 'gle', 'glCreateShader', 'glShaderSource', 'glCompileShader', 'glGetShaderiv', 'glGetShaderInfoLog', 'glCreateProgram', 'glAttachShader', 'glLinkProgram', 'glGetProgramiv', 'glDeleteShader', 'glGetActiveAttrib', 'glGetActiveUniform', 'gl_"', 'glUniform1i', 'glUniform1f', 'glGetUniformLocation', 'glUseProgram', 'glTexStorage2D', 'glTexSubImage2D', 'glGenerateMipmap']

glViewport
glBegin
glEnd
glClear
glVertex3f
*/

  class TextureDrawerGl : public TextureDrawer {
    public:
    virtual void Draw(){
      
    }
  };

  // class SceneRenderGl: public SceneRender {
  //   std::shared_ptr<QuadsArrayGl> screen_qad;
  //   std::shared_ptr<ShaderGl> default_shader;

  //   public:
  //   SceneRenderGl(){
  //     /// "screen_qad" is used to draw 1 full screen frame to screen or another framebuffer
  //     screen_qad = std::make_shared<QuadsArrayGl>(1);

  //     /// "default_shader" is used to draw when no other shaders were provided
  //     /// it is mandatory for OpenGL
  //     default_shader; // TODO
  //   }

  //   virtual void Draw(std::shared_ptr<Scene> scene){
      
  //   }
  // };

  // ======= object maker ====================================================================
  class AccelFactoryGL : public AccelFactory {
    public:
    virtual int InitAccel(const SysOptions & opts){
      // Alpha
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
      //glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA );
      glEnable(GL_ALPHA_TEST);
      glAlphaFunc(GL_GREATER, 0);

      // Depth
      glEnable(GL_DEPTH_TEST);
      glDepthMask(GL_TRUE);
      glDepthFunc(GL_LEQUAL);
      glDepthRange(0.0, 1.0);

      // clear screen
      glViewport(0, 0, opts.screen_width, opts.screen_height);
      glClearColor(0., 0.5, 0.5, 1.);
      glClear(GL_COLOR_BUFFER_BIT);
      glClear(GL_DEPTH_BUFFER_BIT);
      //SDL_GL_SwapWindow(window);
      //glClearColor(0., 0.5, 0.5, 1.);
      //glClear(GL_COLOR_BUFFER_BIT);
      //glClear(GL_DEPTH_BUFFER_BIT);
      return PM_SUCCESS;
    }
    
    virtual std::shared_ptr<Texture> MakeTexture(std::shared_ptr<Image> img){ 
      return make_shared<TextureGl>(img); 
    }
    virtual std::shared_ptr<Shader> MakeShader(const std::string & vert_txt, const std::string & frag_txt){
      std::shared_ptr<ShaderGl> shader = std::make_shared<ShaderGl>();
      int status;
      if((status = shader->LoadVert(vert_txt)) != PM_SUCCESS) return nullptr;
      if((status = shader->LoadFrag(frag_txt)) != PM_SUCCESS) return nullptr;
      if((status = shader->CreateProgram()) != PM_SUCCESS) return nullptr;
      if((status = shader->AddUniformsAuto()) != PM_SUCCESS) return nullptr;
      return shader;
    }
    virtual std::shared_ptr<FrameBuffer> MakeFrameBuffer(const int & size_x, const int & size_y){
      return std::make_shared<FrameBufferGl>(size_x, size_y);
    }
    virtual std::shared_ptr<TextureDrawer> MakeTextureDrawer(){
      return std::make_shared<TextureDrawerGl>(); 
    }
    //virtual std::shared_ptr<SceneRender> MakeSceneRender(){
    //  return std::make_shared<SceneRenderGl>(); 
    //}
  };
};

#endif
