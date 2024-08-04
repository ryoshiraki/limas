#version 330 core

uniform sampler2D u_tex;
uniform vec2 u_res;
uniform vec2 u_dir;

in vec2 v_texcoord;
out vec4 f_color;

vec4 blur5(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
  vec4 color = vec4(0.0);
  vec2 off1 = vec2(1.3333333333333333) * direction;
  color += texture(image, uv) * 0.29411764705882354;
  color += texture(image, uv + (off1 / resolution)) * 0.35294117647058826;
  color += texture(image, uv - (off1 / resolution)) * 0.35294117647058826;
  return color;
}

void main() {
  f_color = blur5(u_tex, v_texcoord, u_res, u_dir);
}
