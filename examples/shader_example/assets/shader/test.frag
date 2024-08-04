#version 400
#resource "util.glsl"

out vec4 out_color;

uniform float frame;

void main() {
    float rad = mod(frame, 360.0) / 360.0 * TWO_PI;
    float hue = cos(rad) * 0.5 + 0.5;
    out_color = vec4(hsv2rgb(vec3(hue, 1, 1)), 1);
}