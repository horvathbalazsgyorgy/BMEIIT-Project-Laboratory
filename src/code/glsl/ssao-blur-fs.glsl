#version 430 core

precision highp float;

in vec2 tex;

uniform struct{
    int resolution;
} material;

uniform sampler2D ssaoNoise;

layout (location = 0) out float fragmentColor;

void main(void) {
    vec2 texelSize = 1.0f / vec2(textureSize(ssaoNoise, 0));
    float result = 0.0f;
    for(int x = -2; x < 2; ++x){
        for(int y = -2; y < 2; ++y){
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoNoise, tex + offset).r;
        }
    }
    const float res = material.resolution;
    fragmentColor = result / (res * res);
}
