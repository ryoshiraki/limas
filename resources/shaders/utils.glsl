#define PI 3.141592653589793
#define TWO_PI PI * 2.
#define EPS 0.0000000001

float atan2(in float y, in float x) {
    return x == 0.0 ? sign(y) * PI / 2 : atan(y, x);
}

vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + EPS)), d / (q.x + EPS), q.x);
}

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float map(float value, float min1, float max1, float min2, float max2) {
    float perc = (value - min1) / (max1 - min1);
    perc = clamp(perc, 0, 1);
    return perc * (max2 - min2) + min2;
}

float cmap(float value, float min1, float max1, float min2, float max2) {
    float perc = (value - min1) / (max1 - min1);
    return perc * (max2 - min2) + min2;
}

vec4 to8bit(uint val) {
    uint low8 = val & 0xFFu;
    uint high8 = (val >> 8) & 0xFFu;
    return vec4(float(low8) / 255.0, float(high8) / 255.0, 0.0, 1.0);
}

uint to16bit(vec4 color) {
    uint low8 = uint(color.r * 255.0);
    uint high8 = uint(color.g * 255.0);
    return (high8 << 8) | low8;
}

mat4 getTranslationMatrix(vec3 position) {
    return mat4(1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                position.x, position.y, position.z, 1.0);
}

mat4 getRotationMatrix(float angle, vec3 axis) {  
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

mat4 getScaleMatrix(vec3 scale) {
    return mat4(scale.x, 0.0,    0.0,    0.0,
                0.0,    scale.y, 0.0,    0.0,
                0.0,    0.0,    scale.z, 0.0,
                0.0,    0.0,    0.0,    1.0);
}

mat4 quatToMat4(vec4 q) {
    float qx = q.x;
    float qy = q.y;
    float qz = q.z;
    float qw = q.w;

    return mat4(
        1.0 - 2.0 * qy * qy - 2.0 * qz * qz, 2.0 * qx * qy - 2.0 * qz * qw,     2.0 * qx * qz + 2.0 * qy * qw,     0.0,
        2.0 * qx * qy + 2.0 * qz * qw,     1.0 - 2.0 * qx * qx - 2.0 * qz * qz, 2.0 * qy * qz - 2.0 * qx * qw,     0.0,
        2.0 * qx * qz - 2.0 * qy * qw,     2.0 * qy * qz + 2.0 * qx * qw,     1.0 - 2.0 * qx * qx - 2.0 * qy * qy, 0.0,
        0.0,                             0.0,                             0.0,                             1.0
    );
}

