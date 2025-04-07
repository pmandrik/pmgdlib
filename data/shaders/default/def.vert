#version 300 es
in vec3 in_Position;
in vec2 a_texCoord;
out vec2 v_texCoord;

void main(void) {
  gl_Position = vec4(in_Position, 1.0);
  v_texCoord = a_texCoord;
}