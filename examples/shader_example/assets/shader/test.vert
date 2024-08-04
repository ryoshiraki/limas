#version 400
#resource "util.glsl"
#include "util.glsl"

layout(location = 0) in vec3 position;

uniform mat4 MVP_MAT;
uniform float frame;

void main() {
    float rad = mod(frame, 180.0) / 180.0 * TWO_PI;
    float x = cos(rad);
    rad = mod(frame, 240.0) / 240.0 * TWO_PI;
    float y = sin(rad);
    gl_Position = MVP_MAT * vec4(position + vec3(x, y, 0), 1);
}