#version 400

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec4 color;

out vec4 v_color;
out vec2 v_texcoord;

uniform mat4 u_mvp;

void main() {
    v_color = color;
    v_texcoord = texcoord;
    gl_Position = u_mvp * vec4(position, 0, 1);
}