#version 430 core

precision highp float;

in vec2 tex;

uniform int mipLevel;
uniform sampler2D srcTexture;

layout (location = 0) out vec3 fragmentColor;

void main(void) {
    vec2 srcTexelSize = 1.0f / vec2(textureSize(srcTexture, mipLevel));
    float x = srcTexelSize.x;
    float y = srcTexelSize.y;

    vec3 a = textureLod(srcTexture, vec2(tex.x - 2*x, tex.y + 2*y), mipLevel).rgb;
    vec3 b = textureLod(srcTexture, vec2(tex.x,       tex.y + 2*y), mipLevel).rgb;
    vec3 c = textureLod(srcTexture, vec2(tex.x + 2*x, tex.y + 2*y), mipLevel).rgb;

    vec3 d = textureLod(srcTexture, vec2(tex.x - 2*x, tex.y), mipLevel).rgb;
    vec3 e = textureLod(srcTexture, vec2(tex.x,       tex.y), mipLevel).rgb;
    vec3 f = textureLod(srcTexture, vec2(tex.x + 2*x, tex.y), mipLevel).rgb;

    vec3 g = textureLod(srcTexture, vec2(tex.x - 2*x, tex.y - 2*y), mipLevel).rgb;
    vec3 h = textureLod(srcTexture, vec2(tex.x,       tex.y - 2*y), mipLevel).rgb;
    vec3 i = textureLod(srcTexture, vec2(tex.x + 2*x, tex.y - 2*y), mipLevel).rgb;

    vec3 j = textureLod(srcTexture, vec2(tex.x - x, tex.y + y), mipLevel).rgb;
    vec3 k = textureLod(srcTexture, vec2(tex.x + x, tex.y + y), mipLevel).rgb;
    vec3 l = textureLod(srcTexture, vec2(tex.x - x, tex.y - y), mipLevel).rgb;
    vec3 m = textureLod(srcTexture, vec2(tex.x + x, tex.y - y), mipLevel).rgb;

    fragmentColor  = e * 0.125f;
    fragmentColor += (a+c+g+i) * 0.03125f;
    fragmentColor += (b+d+f+h) * 0.0625f;
    fragmentColor += (j+k+l+m) * 0.125f;
}
