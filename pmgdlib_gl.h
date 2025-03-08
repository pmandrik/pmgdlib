// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_GL_HH
#define PMGDLIB_GL_HH 1

#include "pmgdlib_math.h"
//#include <GL/gl.h>
//#include <GL/glext.h>

namespace pmgd {

  /// Dependencies:
  /// glBegin
  /// GL_QUADS
  /// glVertex3f
  /// glTexCoord2f

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

  void draw_textured_quad(const v2 & pos, v2 size, const v2 & tpos, const v2 & tsize, const float & angle, const v2 & flip){
    /// TODO test
    if(flip.x) size.x *= -1;
    if(flip.y) size.y *= -1;

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
  void fill_verices_array(float * vertices, const int & index, v2 & pos, v2 size, const float & angle){
    v2 perp = v2(-size.x, size.y);
    size = size.Rotated(angle);
    perp = perp.Rotated(angle);

    vertices[index]   = pos.x - size.x;
    vertices[index+1] = pos.y + size.y;
    vertices[index+2] = pos.z;

    vertices[index+3] = pos.x - perp.x;
    vertices[index+4] = pos.y + perp.y;
    vertices[index+5] = pos.z;

    vertices[index+6] = pos.x + size.x;
    vertices[index+7] = pos.y - size.y;
    vertices[index+8] = pos.z;

    vertices[index+9]  = pos.x + perp.x;
    vertices[index+10] = pos.y - perp.y;
    vertices[index+11] = pos.z;
  }

  void draw_quad_array(float * vertices, int n_quads){
    int n_indexes = n_quads*6;
    int * indices = new int[n_indexes];

    for(int i = 0; i < n_indexes; i++){
      int vi = i / 6;
      int qi = i % 6;
      indices[i] = vi * 4 + qi - (qi>2) - 4*(qi==5);
      // printf("%d,", indices[i]);
      // if(qi == 5) printf("\n");
    }
    // printf("\n");

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawElements(GL_TRIANGLES, n_quads*6, GL_UNSIGNED_INT, indices);
    glDisableClientState(GL_VERTEX_ARRAY);

    delete[] indices;
  }

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
