#version 400

in vec2 v_texcoord;
out vec4 f_color;

uniform sampler2D u_tex;
uniform bool u_YCoCg;

const vec4 offsets = vec4(-0.50196078431373, -0.50196078431373, 0.0, 0.0);

void main() {
    vec4 col = texture(u_tex, v_texcoord);
    if (u_YCoCg) {
        col += offsets;
        float scale = ( col.z * ( 255.0 / 8.0 ) ) + 1.0;
        float Co = col.x / scale;
        float Cg = col.y / scale;
        float Y = col.w;
        f_color = vec4(Y + Co - Cg, Y + Cg, Y - Co - Cg, 1.0);
    } else {
        f_color = col;
    }
}