
#version 330 core
  
in vec2 v_texcoord;
out vec4 f_color;

uniform sampler2D u_tex;
uniform vec2 u_res_inv;
uniform int u_horizontal;
uniform int u_radius;
uniform float u_weights[25];

void main() {             
    vec3 result = texture(u_tex, v_texcoord).rgb;
    if(u_horizontal == 0) {
        result *= u_weights[0];
        for(int i = 0; i < u_radius; ++i) {
            result += texture(u_tex, v_texcoord + vec2(u_res_inv.x * i, 0.0)).rgb * u_weights[i + 1];
            result += texture(u_tex, v_texcoord - vec2(u_res_inv.x * i, 0.0)).rgb * u_weights[i + 1];
        }
    } else if(u_horizontal == 1) {
        result *= u_weights[0];
        for(int i = 0; i < u_radius; ++i) {
            result += texture(u_tex, v_texcoord + vec2(0.0, u_res_inv.y * i)).rgb * u_weights[i + 1];
            result += texture(u_tex, v_texcoord - vec2(0.0, u_res_inv.y * i)).rgb * u_weights[i + 1];
        }
    }
    f_color = vec4(result, 1.0);
}