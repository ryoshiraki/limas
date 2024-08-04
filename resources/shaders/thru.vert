#version 400

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 texcoord;

out vec4 v_color;
out vec2 v_texcoord;
void main() {
    v_color = color;
    v_texcoord = position.xy * 0.5 + 0.5;
    // v_texcoord.y = 1 - v_texcoord.y;
    gl_Position = vec4(position, 1);
}