#version 400

in vec4 v_color; 
in vec2 v_texcoord;
out vec4 o_color;

uniform sampler2D u_tex;

void main() {
    o_color = texture(u_tex, v_texcoord) * v_color;
}