#version 400

out vec4 f_color;

uniform vec2 u_resolution;
uniform vec2 u_mouse;

float dist_func(vec3 pos, float size) {
    return length(pos) - size;
}

vec3 getNormal(vec3 pos, float size)
{
    float ep = 0.0001;
    return normalize(vec3(
            dist_func(pos, size) - dist_func(vec3(pos.x - ep, pos.y, pos.z), size),
            dist_func(pos, size) - dist_func(vec3(pos.x, pos.y - ep, pos.z), size),
            dist_func(pos, size) - dist_func(vec3(pos.x, pos.y, pos.z - ep), size)
        ));
}

vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));

void main() {
  vec2 pos = (gl_FragCoord.xy * 2.0 - u_resolution.xy) / min(u_resolution.x, u_resolution.y);
    vec3 col = vec3(0.0);
    vec3 cameraPos = vec3(0.0, 0.0, 10.0);
    vec3 ray = normalize(vec3(pos, 0.0) - cameraPos);
    vec3 cur = cameraPos;

    vec2 mouseNorm = u_mouse / u_resolution * 2.0 - 1.0;
    float size = 1.0 - length(mouseNorm);
    for (int i = 0; i < 16; i++)
    {
        float d = dist_func(cur, size);
        if (d < 0.0001)
        {
            vec3 normal = getNormal(cur, size);
            float diff = dot(normal, lightDir);
            col = vec3(diff) + vec3(0.1);
            break;
        }
        cur += ray * d;
    }
//  f_color = texture(u_tex, v_texcoord) * v_color;
  f_color = vec4(col, 1);
}

