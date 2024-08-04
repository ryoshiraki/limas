#version 330 core

const vec3 LumCoeff = vec3 (0.2125, 0.7154, 0.0721);

uniform sampler2D u_tex;
uniform vec2  u_res;

uniform float u_brightness;
uniform float u_contrast;
uniform float u_saturation;
uniform float u_invert;
uniform float u_avgluma;
uniform float u_alpha;

in vec2 v_texcoord;
out vec4 f_color;

void main() {  
  vec4 col = texture(u_tex, v_texcoord);
  vec3 rgb = col.rgb;
  vec3 intensity = vec3(dot(rgb, LumCoeff));
  rgb = mix(intensity, rgb, u_saturation);
  rgb = mix(vec3(u_avgluma), rgb, u_contrast);
  rgb *= u_brightness;
  vec3 gray = mix(intensity, rgb, vec3(0));
  if(u_invert > 0.0){
    vec3 inverted = vec3(1, 1, 1) - rgb;
    inverted *= u_invert;
    rgb *= (1.0 - u_invert);
    rgb += inverted;
  }
  
  f_color = vec4(rgb, u_alpha);
;
}
