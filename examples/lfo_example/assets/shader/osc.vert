#version 400
#resource "utils.glsl"

layout(location = 0)in float amp;

uniform int u_size;
void main() {
  int i = gl_VertexID;
  float x = cmap(i, 0, u_size, - 1, 1);
  gl_Position = vec4(x, amp, 0, 1);
}