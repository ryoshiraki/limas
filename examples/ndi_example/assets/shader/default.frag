#version 400

in vec2 v_texcoord;
in vec4 v_color;
out vec4 f_color;

uniform sampler2D u_tex;

void main() {
//  f_color = texture(u_tex, v_texcoord) * v_color;
  f_color = v_color;
}

