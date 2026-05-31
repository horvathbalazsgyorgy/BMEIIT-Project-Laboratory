#version 430 core

precision highp float;

in vec4 rayDir;

uniform struct{
    samplerCube envTexture;
} material;

layout (location = 0) out vec3 fragmentColor;

void main(void) {
    fragmentColor = textureLod(material.envTexture, rayDir.xyz, 0.0).rgb;
}
