#version 430 core

precision highp float;

in vec4 rayDir;

uniform struct{
    samplerCube envTexture;
} material;

out vec4 fragmentColor;

void main(void) {
    vec3 color = texture(material.envTexture, rayDir.xyz).rgb;
    fragmentColor = vec4(color, 1.0f);
}
