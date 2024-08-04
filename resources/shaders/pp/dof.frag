#version 330
#define PI  3.14159265

uniform sampler2D u_color_tex;
uniform sampler2D u_depth_tex;

in vec2 v_texcoord;
out vec4 f_color;

uniform vec2 u_res;
uniform float u_near; //camera clipping start
uniform float u_far; //camera clipping end

float width = u_res.x; //texture width
float height = u_res.y; //texture height
vec2 texel = 1.0 / u_res;

int samples = 3; //samples on the first ring
int rings = 3; //ring count

uniform struct Focus {
	float depth;
	float length;
	float fstop;
	float coc;
};
uniform Focus u_focus;

uniform struct AutoFocus {
	bool enable;
	vec2 center;
};
uniform AutoFocus u_autofocus;

uniform struct ManualDof {
	bool enable;
	float near_start; //near dof blur start
	float near_dist; //near dof blur falloff distance
	float far_start; //far dof blur start
	float far_dist; //far dof blur falloff distance
};
uniform ManualDof u_manual_dof;

uniform struct Blur {
	float size; //clamp value of max blur (0.0 = no blur,1.0 default)
	float falloff; //bokeh edge bias
};
uniform Blur u_blur;

uniform struct DepthBlur {
	bool enable;
	float size;
};
uniform DepthBlur u_depth_blur;

float linearize(float depth) {
    return -u_far * u_near / (depth * (u_far - u_near) - u_far);
}

float bdepth(vec2 coords) { //blurring depth

    float d = 0.0;
    float kernel[9];
    vec2 offset[9];
    
    vec2 wh = texel * u_depth_blur.size;
    
    offset[0] = vec2(-wh.x,-wh.y);
    offset[1] = vec2( 0.0, -wh.y);
    offset[2] = vec2( wh.x -wh.y);
    
    offset[3] = vec2(-wh.x,  0.0);
    offset[4] = vec2( 0.0,   0.0);
    offset[5] = vec2( wh.x,  0.0);
    
    offset[6] = vec2(-wh.x, wh.y);
    offset[7] = vec2( 0.0,  wh.y);
    offset[8] = vec2( wh.x, wh.y);
    
    kernel[0] = 1.0/16.0;   kernel[1] = 2.0/16.0;   kernel[2] = 1.0/16.0;
    kernel[3] = 2.0/16.0;   kernel[4] = 4.0/16.0;   kernel[5] = 2.0/16.0;
    kernel[6] = 1.0/16.0;   kernel[7] = 2.0/16.0;   kernel[8] = 1.0/16.0;
    
    for( int i = 0; i < 9; i++ ) {
        float tmp = texture(u_depth_tex, coords + offset[i]).r;
        d += tmp * kernel[i];
    }
    
    return d;
}

void main() {
    float depth = u_depth_blur.enable ? 
	linearize(bdepth(v_texcoord)) : linearize(texture(u_depth_tex, v_texcoord).x);
	
 	float focal_depth = u_autofocus.enable ? 
	linearize(texture(u_depth_tex, u_autofocus.center).x) : u_focus.depth;

	float blur = 0.0;
	if (u_manual_dof.enable) {    
		float a = depth - focal_depth; //focal plane
		float b = (a - u_manual_dof.far_start) / u_manual_dof.far_dist; //far DoF
		float c = (-a - u_manual_dof.near_start) / u_manual_dof.near_dist; //near Dof
		blur = (a > 0.0) ? b : c;
	} else {
		float f = u_focus.length; //focal length in mm
		float d = focal_depth * 1000.0;// focal plane in mm
		float o = depth * 1000.0f;// depth in mm
		
		float a = (o * f) / (o - f); 
		float b = (d * f) / (d - f); 
		float c = (d - f) / (d * u_focus.fstop * u_focus.coc); 
		blur = abs(a - b) * c;
	}

 	// blur = clamp(blur, 0.0, 1.0);

	float w = texel.x * blur * u_blur.size;
	float h = texel.y * blur * u_blur.size;

    f_color = texture(u_color_tex, v_texcoord);
	if(blur > 0.0) {
		float s = 1.0;
		int ring_samples;
		for (int i = 1; i <= rings; i += 1) {   
			ring_samples = i * samples;
			for (int j = 0 ; j < ring_samples ; j += 1) {
				float step = PI * 2.0 / float(ring_samples);
				float pw = (cos(float(j) * step) * float(i));
				float ph = (sin(float(j) * step) * float(i));
				f_color.rgb += texture(u_color_tex, v_texcoord.xy + vec2(pw * w, ph * h), blur).rgb * mix(1.0, (float(i))/(float(rings)), u_blur.falloff);  
				s += 1.0 * mix(1.0, (float(i)) / (float(rings)), u_blur.falloff);   
			}
		}
		f_color.rgb /= s;
	}    
}







// #version 330 core

// uniform sampler2D u_color_tex;
// uniform sampler2D u_depth_tex;

// uniform vec2 u_res;

// uniform float u_intensity;  // max blur amount
// uniform float u_aperture; // u_aperture - bigger values for shallower depth of field

// uniform float u_focus;
// uniform float aspect;

// in vec2 v_texcoord;
// out vec4 f_color;

// void main() {                        
//     vec2 inv_res= vec2(1.0);/// u_res;
//     float factor = texture(u_depth_tex, v_texcoord ).x - u_focus;
    
//     vec2 dofblur = vec2(clamp( factor * u_aperture, -u_intensity, u_intensity ) );
    
//     vec2 dofblur9 = dofblur * 0.9;
//     vec2 dofblur7 = dofblur * 0.7;
//     vec2 dofblur4 = dofblur * 0.4;
    
//     vec4 col =vec4(0.0);
    
//     col += texture(u_color_tex, v_texcoord );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.0,   0.4  ) * inv_res ) * dofblur );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.15,  0.37 ) * inv_res ) * dofblur );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.29,  0.29 ) * inv_res ) * dofblur );
//     col += texture(u_color_tex, v_texcoord + ( vec2( -0.37,  0.15 ) * inv_res ) * dofblur );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.40,  0.0  ) * inv_res ) * dofblur );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.37, -0.15 ) * inv_res ) * dofblur );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.29, -0.29 ) * inv_res ) * dofblur );
//     col += texture(u_color_tex, v_texcoord + ( vec2( -0.15, -0.37 ) * inv_res ) * dofblur );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.0,  -0.4  ) * inv_res ) * dofblur );
//     col += texture(u_color_tex, v_texcoord + ( vec2( -0.15,  0.37 ) * inv_res ) * dofblur );
//     col += texture(u_color_tex, v_texcoord + ( vec2( -0.29,  0.29 ) * inv_res ) * dofblur );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.37,  0.15 ) * inv_res ) * dofblur );
//     col += texture(u_color_tex, v_texcoord + ( vec2( -0.4,   0.0  ) * inv_res ) * dofblur );
//     col += texture(u_color_tex, v_texcoord + ( vec2( -0.37, -0.15 ) * inv_res ) * dofblur );
//     col += texture(u_color_tex, v_texcoord + ( vec2( -0.29, -0.29 ) * inv_res ) * dofblur );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.15, -0.37 ) * inv_res ) * dofblur );
    
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.15,  0.37 ) * inv_res ) * dofblur9 );
//     col += texture(u_color_tex, v_texcoord + ( vec2( -0.37,  0.15 ) * inv_res ) * dofblur9 );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.37, -0.15 ) * inv_res ) * dofblur9 );
//     col += texture(u_color_tex, v_texcoord + ( vec2( -0.15, -0.37 ) * inv_res ) * dofblur9 );
//     col += texture(u_color_tex, v_texcoord + ( vec2( -0.15,  0.37 ) * inv_res ) * dofblur9 );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.37,  0.15 ) * inv_res ) * dofblur9 );
//     col += texture(u_color_tex, v_texcoord + ( vec2( -0.37, -0.15 ) * inv_res ) * dofblur9 );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.15, -0.37 ) * inv_res ) * dofblur9 );
    
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.29,  0.29 ) * inv_res ) * dofblur7 );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.40,  0.0  ) * inv_res ) * dofblur7 );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.29, -0.29 ) * inv_res ) * dofblur7 );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.0,  -0.4  ) * inv_res ) * dofblur7 );
//     col += texture(u_color_tex, v_texcoord + ( vec2( -0.29,  0.29 ) * inv_res ) * dofblur7 );
//     col += texture(u_color_tex, v_texcoord + ( vec2( -0.4,   0.0  ) * inv_res ) * dofblur7 );
//     col += texture(u_color_tex, v_texcoord + ( vec2( -0.29, -0.29 ) * inv_res ) * dofblur7 );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.0,   0.4  ) * inv_res ) * dofblur7 );
    
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.29,  0.29 ) * inv_res ) * dofblur4 );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.4,   0.0  ) * inv_res ) * dofblur4 );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.29, -0.29 ) * inv_res ) * dofblur4 );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.0,  -0.4  ) * inv_res ) * dofblur4 );
//     col += texture(u_color_tex, v_texcoord + ( vec2( -0.29,  0.29 ) * inv_res ) * dofblur4 );
//     col += texture(u_color_tex, v_texcoord + ( vec2( -0.4,   0.0  ) * inv_res ) * dofblur4 );
//     col += texture(u_color_tex, v_texcoord + ( vec2( -0.29, -0.29 ) * inv_res ) * dofblur4 );
//     col += texture(u_color_tex, v_texcoord + ( vec2(  0.0,   0.4  ) * inv_res ) * dofblur4 );
    
//     f_color = col / 41.0;
//     f_color.a = 1.0;
// }