#version 430 core

precision highp float;

in vec4 rayDir;

uniform struct{
    float exposure;
    samplerCube envTexture;
} material;

out vec4 fragmentColor;

void main(void) {
    vec3 color = textureLod(material.envTexture, rayDir.xyz, 0.5).rgb;
    vec3 toneMapped = vec3(1.0f) - exp(-color * material.exposure);
    fragmentColor = vec4(toneMapped, 1.0f);
}
