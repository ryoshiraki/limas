#version 330 core

const vec3 LUMA = vec3( 0.299, 0.587, 0.114 );
const float FXAA_REDUCE_MIN = 1.0/128.0;
const float FXAA_REDUCE_MUL = 1.0/8.0;
const float FXAA_SPAN_MAX = 8.0;

uniform sampler2D u_tex;
uniform vec2 u_res;
vec2 texel = 1.0 / u_res;

in vec2 v_texcoord;
out vec4 f_color;

void main() {
    
    vec3 rgbNW = texture(u_tex, (gl_FragCoord.xy + vec2(-1.0, -1.0)) * texel).xyz;
    vec3 rgbNE = texture(u_tex, (gl_FragCoord.xy + vec2(1.0, -1.0)) * texel).xyz;
    vec3 rgbSW = texture(u_tex, (gl_FragCoord.xy + vec2(-1.0, 1.0)) * texel).xyz;
    vec3 rgbSE = texture(u_tex, (gl_FragCoord.xy + vec2(1.0, 1.0)) * texel).xyz;
    vec4 rgbaM = texture(u_tex, gl_FragCoord.xy  * texel);
    vec3 rgbM = rgbaM.xyz;
    float opacity  = rgbaM.w;
    
    float lumaNW = dot( rgbNW, LUMA );
    float lumaNE = dot( rgbNE, LUMA );
    float lumaSW = dot( rgbSW, LUMA );
    float lumaSE = dot( rgbSE, LUMA );
    float lumaM  = dot( rgbM,  LUMA );
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE) , max(lumaSW, lumaSE)));
    
    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * texel;
    
    vec3 rgbA = 0.5 * (texture(u_tex, gl_FragCoord.xy  * texel + dir * (1.0 / 3.0 - 0.5)).xyz + texture(u_tex, gl_FragCoord.xy  * texel + dir * (2.0 / 3.0 - 0.5)).xyz);
    vec3 rgbB = rgbA * 0.5 + 0.25 * (texture(u_tex, gl_FragCoord.xy  * texel + dir * -0.5).xyz + texture( u_tex, gl_FragCoord.xy  * texel + dir * 0.5).xyz);
    
    float lumaB = dot(rgbB, luma);
    
    if ((lumaB < lumaMin) || (lumaB > lumaMax)) {
        f_color = vec4( rgbA, opacity );   
    } else {
        f_color = vec4( rgbB, opacity );
    }
}