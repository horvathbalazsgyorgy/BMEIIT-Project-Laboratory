#version 430 core

precision highp float;

in vec4 rayDir;

uniform struct{
    sampler2D equirectangularMap;
} material;

out vec4 fragmentColor;
const vec2 invAtan = vec2(0.1591f, 0.3183f);

vec2 sphericalToCartesian(vec3 direction){
    vec2 uv = vec2(atan(direction.z, direction.x), asin(direction.y));
    uv *= invAtan;
    uv += 0.5f;
    return uv;
}

void main(void) {
    vec2 uv = sphericalToCartesian(normalize(rayDir.xyz));
    vec3 color = texture(material.equirectangularMap, uv).rgb;

    fragmentColor = vec4(color, 1.0f);
}
