#version 430 core

precision highp float;

in vec4 rayDir;

uniform struct{
    sampler2D panoramaLDR;
    sampler2D panoramaHDR;
} material;

layout (location = 0) out vec4 ldrColor;
layout (location = 1) out vec4 hdrColor;
const float PI = 3.14159265359;
const vec2 invAtan = vec2(1.0/(2.0 * PI), 1.0/PI);

vec2 sphericalToCartesian(vec3 direction){
    vec2 uv = vec2(atan(direction.z, direction.x), asin(direction.y));
    uv *= invAtan;
    uv += 0.5f;
    return uv;
}

void main(void) {
    vec2 uv = sphericalToCartesian(normalize(rayDir.xyz));
    vec3 ldr = texture(material.panoramaLDR, uv).rgb;
    vec3 hdr = texture(material.panoramaHDR, uv).rgb;

    ldrColor = vec4(ldr, 1.0f);
    hdrColor = vec4(hdr, 1.0f);
}
