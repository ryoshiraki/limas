#version 400
#resource "utils.glsl"

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 texcoord;
layout(location = 4) in vec3 translation;
layout(location = 5) in vec4 rotation;
layout(location = 6) in vec3 scale;
layout(location = 7) in vec4 instance_color;

out vec4 v_color;
out vec2 v_texcoord;

uniform mat4 MVP_MAT;
uniform mat4 TEX_MAT; 
uniform vec4 COLOR;
uniform bool HAS_COLOR;
uniform bool HAS_NORMAL;
uniform bool HAS_TEXCOORD;
uniform bool INSTANCED;

void main() {
    v_color = COLOR;
    v_texcoord = (TEX_MAT * vec4(texcoord, 0, 1)).xy;

    if (HAS_COLOR) {
        v_color *= color;
    }

    if (INSTANCED) {
        v_color *= instance_color;
        mat4 translation_mat = getTranslationMatrix(translation);
        mat4 rotation_mat = quatToMat4(rotation);
        mat4 scale_mat = getScaleMatrix(scale);
        mat4 model_mat = translation_mat * rotation_mat * scale_mat;
        gl_Position = MVP_MAT * model_mat * vec4(position, 1);
    } else {
        gl_Position = MVP_MAT * vec4(position, 1);
    }
}