#version 430 core

precision highp float;

in vec2 tex;

uniform int mipLevel;
uniform sampler2D srcTexture;

const float filterRadius = 0.005f;
layout (location = 0) out vec3 fragmentColor;

void main(void) {
    vec2 srcTexelSize = textureSize(srcTexture, mipLevel);

    float x = 1.0f / srcTexelSize.x;
    float y = 1.0f / srcTexelSize.y;

    vec3 a = textureLod(srcTexture, vec2(tex.x - x, tex.y + y), mipLevel).rgb;
    vec3 b = textureLod(srcTexture, vec2(tex.x,     tex.y + y), mipLevel).rgb;
    vec3 c = textureLod(srcTexture, vec2(tex.x + x, tex.y + y), mipLevel).rgb;

    vec3 d = textureLod(srcTexture, vec2(tex.x - x, tex.y), mipLevel).rgb;
    vec3 e = textureLod(srcTexture, vec2(tex.x,     tex.y), mipLevel).rgb;
    vec3 f = textureLod(srcTexture, vec2(tex.x + x, tex.y), mipLevel).rgb;

    vec3 g = textureLod(srcTexture, vec2(tex.x - x, tex.y - y), mipLevel).rgb;
    vec3 h = textureLod(srcTexture, vec2(tex.x,     tex.y - y), mipLevel).rgb;
    vec3 i = textureLod(srcTexture, vec2(tex.x + x, tex.y - y), mipLevel).rgb;

    fragmentColor  = e * 4.0f;
    fragmentColor += (b+d+f+h) * 2.0f;
    fragmentColor += (a+c+g+i);
    fragmentColor *= 1.0f / 16.0f;
}
