#version 430 core

precision highp float;

in vec4 rayDir;

uniform struct{
    samplerCube envTexture;
} material;

uniform float exposure;

out vec4 fragmentColor;

void main(void) {
    vec3 color = texture(material.envTexture, rayDir.xyz).rgb;
    vec3 toneMapped = vec3(1.0f) - exp(-color * exposure);

    fragmentColor = vec4(toneMapped, 1.0f);
}
