#version 410

uniform sampler3D volumeTexture;
in vec3 v_position;
out vec4 fragColor;

void main() {
    vec3 rayDir = normalize(v_position - cameraPos);
    vec3 currentPos = v_position;
    for(int i = 0; i < MAX_STEPS; i++) {
        float density = texture(volumeTexture, currentPos).r;
        vec4 color = vec4(density); // 簡単のために密度をそのまま色に使用します
        fragColor += color * (1.0 - fragColor.a); // アルファ合成
        if(fragColor.a >= 0.95) break; // 早期終了
        currentPos += rayDir * STEP_SIZE;
    }
}
