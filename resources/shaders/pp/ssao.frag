#version 330 core


//const float PI = 3.14159265;
const float DL = 2.399963229728653; // PI * ( 3.0 - sqrt( 5.0 ) )
const float EULER = 2.718281828459045;

in vec2 v_texcoord;
out vec4 f_color;

uniform sampler2D u_col_tex;
uniform sampler2D u_depth_tex;
uniform vec2 u_res;			// texture width, height

uniform float u_cam_near;
uniform float u_cam_far;

uniform float u_fog_near;
uniform float u_fog_far;

uniform bool u_fog_enabled;		// attenuate AO with linear fog
uniform bool u_only_ao; 		// use only ambient occlusion pass?

uniform float u_ao_clamp; 		// depth clamp - reduces haloing at screen edges

uniform float u_lum_influence;  // how much luminance affects occlusion

// user variables

const int AO_SAMPLES = 8; 		// ao sample count
const float AO_RADIUS = 5.0; 	// ao AO_RADIUS

const bool USE_NOISE = false; 		 // use noise instead of pattern for sample dithering
const float NOISE_AMOUNT = 0.0003; // dithering amount

const float DIFF_AREA = 0.4; 		// self-shadowing reduction
const float G_DISPLACE = 0.4; 	// gauss bell center

const vec3 AO_COLOR = vec3( 1.0, 0.7, 0.5 );
//const vec3 AO_COLOR = vec3( 1.0, 1.0, 1.0 );,


// RGBA depth

float unpackDepth( const in vec4 rgba_depth ) {
    const vec4 bit_shift = vec4( 1.0 / ( 256.0 * 256.0 * 256.0 ), 1.0 / ( 256.0 * 256.0 ), 1.0 / 256.0, 1.0 );
    float depth = dot( rgba_depth, bit_shift );
    return depth;
}

// generating noise / pattern texture for dithering

vec2 rand( const vec2 coord ) {
    vec2 noise;
    if ( USE_NOISE ) {
        float nx = dot ( coord, vec2( 12.9898, 78.233 ) );
        float ny = dot ( coord, vec2( 12.9898, 78.233 ) * 2.0 );
        noise = clamp( fract ( 43758.5453 * sin( vec2( nx, ny ) ) ), 0.0, 1.0 );        
    } else {        
        float ff = fract( 1.0 - coord.s * ( u_res.x / 2.0 ) );
        float gg = fract( coord.t * ( u_res.y / 2.0 ) );
        noise = vec2( 0.25, 0.75 ) * vec2( ff ) + vec2( 0.75, 0.25 ) * gg;
    }
    return ( noise * 2.0  - 1.0 ) * NOISE_AMOUNT;
}

float doFog() {
    float zdepth = unpackDepth( texture2D( u_depth_tex, v_texcoord ) );
    float depth = -u_cam_far * u_cam_near / ( zdepth * (u_cam_far - u_cam_near) - u_cam_far );
    return smoothstep( u_fog_near, u_fog_far, depth );
}

float readDepth( const in vec2 uv ) {
    //return ( 2.0 * u_cam_near ) / ( u_cam_far + u_cam_near - unpackDepth( texture2D( u_depth_tex, coord ) ) * ( u_cam_far - u_cam_near ) );,
    return (2.0 * u_cam_near) / ((u_cam_far + u_cam_near) - unpackDepth(texture2D(u_depth_tex, uv)) * (u_cam_far - u_cam_near));
}

float compareDepths( const in float depth1, const in float depth2, inout int far ) {

    float garea = 2.0; 						 // gauss bell width
    float diff = ( depth1 - depth2 ) * 100.0; // depth difference (0-100)

    // reduce left bell width to avoid self-shadowing

    if ( diff < G_DISPLACE ) {
        garea = DIFF_AREA;        
    } else {
        far = 1;        
    }

    float dd = diff - G_DISPLACE;
    float gauss = pow( EULER, -2.0 * dd * dd / ( garea * garea ) );
    return gauss;
}

float calcAO( float depth, float dw, float dh ) {
    float dd = AO_RADIUS - depth * AO_RADIUS;
    vec2 vv = vec2( dw, dh );
    vec2 coord1 = v_texcoord + dd * vv;
    vec2 coord2 = v_texcoord - dd * vv;

    float temp1 = 0.0;
    float temp2 = 0.0;
    int far = 0;
    temp1 = compareDepths( depth, readDepth( coord1 ), far );

    // DEPTH EXTRAPOLATION

    if ( far > 0 ) {
        temp2 = compareDepths( readDepth( coord2 ), depth, far );
        temp1 += ( 1.0 - temp1 ) * temp2;
    }
    return temp1;
}
void main() {
    vec2 noise = rand( v_texcoord );
    float depth = readDepth( v_texcoord );
    float tt = clamp( depth, u_ao_clamp, 1.0 );
    float w = ( 1.0 / u_res.x )  / tt + ( noise.x * ( 1.0 - noise.x ) );
    float h = ( 1.0 / u_res.y ) / tt + ( noise.y * ( 1.0 - noise.y ) );
    float pw;
    float ph;
    float ao;
    float dz = 1.0 / float( AO_SAMPLES );
    float z = 1.0 - dz / 2.0;
    float l = 0.0;
    for ( int i = 0; i <= AO_SAMPLES; i ++ ) {
        float r = sqrt( 1.0 - z );
        pw = cos( l ) * r;
        ph = sin( l ) * r;
        ao += calcAO( depth, pw * w, ph * h );
        z = z - dz;
        l = l + DL;
    }
    ao /= float( AO_SAMPLES );
    ao = 1.0 - ao;
    if ( u_fog_enabled ) {
        ao = mix( ao, 1.0, doFog() );
    }
    vec3 color = texture2D( u_col_tex, v_texcoord ).rgb;
    vec3 lumcoeff = vec3( 0.299, 0.587, 0.114 );
    float lum = dot( color.rgb, lumcoeff );
    vec3 luminance = vec3( lum );
    vec3 final = vec3( color * mix( vec3( ao ), vec3( 1.0 ), luminance * u_lum_influence ) );
    if (u_only_ao) {
        final = AO_COLOR * vec3( mix( vec3( ao ), vec3( 1.0 ), luminance * u_lum_influence ) );
    }
    f_color = vec4( final, 1.0 );
}
