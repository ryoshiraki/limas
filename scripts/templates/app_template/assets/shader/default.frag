#version 400

in vec4 v_color; 
in vec2 v_texcoord;
out vec4 f_color;

uniform bool USE_TEX; 
uniform sampler2D TEX;

void main() {
    f_color = USE_TEX ? texture(TEX, v_texcoord) * v_color : v_color;
}