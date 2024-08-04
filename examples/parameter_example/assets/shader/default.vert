#version 400

//default attributes
layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec3 normal;

//default uniforms
uniform mat4 u_mvp_mat;
uniform mat4 u_tex_mat;
uniform vec4 u_color;

out vec4 v_color;
out vec2 v_texcoord;

void main() {
  v_color = color * u_color;
  v_texcoord = (u_tex_mat * vec4(texcoord, 0, 1)).xy;
  gl_Position = u_mvp_mat * vec4(position, 1);
}
