#version 400
#resource "utils.glsl"

layout(location = 0)in vec3 position;
layout(location = 1)in vec3 normal;
layout(location = 2)in vec4 color;
layout(location = 3)in vec2 texcoord;

out vec4 v_color;

uniform mat4 u_mvp;

void main() {
  v_color = color;
  gl_Position = u_mvp * vec4(position, 1);
}