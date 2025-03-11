// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_GL_HH
#define PMGDLIB_GL_HH 1

#include "pmgdlib_defs.h"
#include "pmgdlib_math.h"
#include "pmgdlib_image.h"
#include "pmgdlib_core.h"

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

  void gl_check_error(const char * fname){
    GLenum code;
    while((code = glGetError()) != GL_NO_ERROR){
      // msg_err(fname, " error: ", code); TODO
      std::cerr << "Error: " << code << std::endl;
    }
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

    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
      printf("errors: %d", err);
    }
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
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_indexes*sizeof(GLint), indexes, GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLES, n_indexes, GL_UNSIGNED_INT, (GLvoid*)0);

    // tear down
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &VBOId);
    glDeleteVertexArrays(1, &VAOId);

    glDeleteBuffers(1, &elementbuffer);
    glDeleteVertexArrays(1, &elementbuffer);
  }

  // ======= texture ====================================================================
  struct TextureDrawData {
    v2 pos, size;
    v2 tpos = v2(0,0), tsize = v2(1,1);
    float angle = 0;
    bool flip_x = false, flip_y = false;

    TextureDrawData(v2 pos, v2 size){
      this->pos = pos;
      this->size = size;
    }
  };

  GLuint image_format_to_gl(int format){
    if(format == image_format::RGBA) return GL_RGBA;
    return GL_RGBA;
  };

  GLuint image_type_to_gl(int type){
    if(type == image_type::UNSIGNED_INT) return GL_UNSIGNED_INT;
    if(type == image_type::FLOAT) return GL_FLOAT;
    return GL_UNSIGNED_INT;
  };

  class TextureDrawer {
    /// thread unsafe
    private :
      GLuint id, type, format, internalformat;
      bool is_binded = false;

    public:
      TextureDrawer(std::shared_ptr<Image> img){
        // MSG_INFO( __PFN__, "from Image", img ); TODO
        glGenTextures(1, &id);

        format = image_format_to_gl(img->format);
        type   = image_type_to_gl(img->type);
        internalformat = format;

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
        Unbind();
      }

    public:
      void Bind()  { if(    is_binded) return; glBindTexture(GL_TEXTURE_2D, id); is_binded = true;  }
      void Unbind(){ if(not is_binded) return; glBindTexture(GL_TEXTURE_2D,  0); is_binded = false; }

      void Draw(const TextureDrawData & data){
        Bind();
        glEnable(GL_TEXTURE_2D);
        draw_textured_quad(data.pos, data.size, data.tpos, data.tsize, data.angle, data.flip_x, data.flip_y);
        Unbind();
      }
  };

/*
['glMatrixMode', 'glLoadMatrixf', 'glLoadIdentity', 'glOrtho', '', 'glGetFloatv', 'glRotatef', 'glTranslatef', 'gluLookAt', 'gle){', '', 'gle);', '', '', 'gle,', 'glTexCoord2f', 'glEnableClientState', 'glVertexPointer', 'glTexCoordPointer', 'glEnable', 'glDrawArrays', 'gle=0,', 'glActiveTexture', 'glGenTextures', 'glBindTexture', 'glTexParameteri', 'glTexImage2D', 'glGenRenderbuffers', 'glBindRenderbuffer', 'glRenderbufferStorage', 'glGenFramebuffers', 'glBindFramebuffer', 'glFramebufferTexture2D', 'glFramebufferRenderbuffer', 'glCheckFramebufferStatus', 'glCheckFramebufferStatus"', 'glClearColor', '', '', 'gl_check_error', 'glGetError', 'glTexImage2D_err', 'gle', 'glCreateShader', 'glShaderSource', 'glCompileShader', 'glGetShaderiv', 'glGetShaderInfoLog', 'glCreateProgram', 'glAttachShader', 'glLinkProgram', 'glGetProgramiv', 'glDeleteShader', 'glGetActiveAttrib', 'glGetActiveUniform', 'gl_"', 'glUniform1i', 'glUniform1f', 'glGetUniformLocation', 'glUseProgram', 'glTexStorage2D', 'glTexSubImage2D', 'glGenerateMipmap']

glViewport
glBegin
glEnd
glClear
glVertex3f
*/

};

#endif
