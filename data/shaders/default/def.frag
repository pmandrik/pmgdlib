#version 300 es
precision mediump float;
in vec2 v_texCoord;
uniform sampler2D text_0;
out vec4 fragColor;
void main(){
  fragColor = texture2D(text_0, v_texCoord);
}