#version 430 core

precision highp float;

in vec2 tex;

uniform sampler2D scene;

layout (location = 0) out vec3 fragmentColor;

void main(void) {
    fragmentColor = texture(scene, tex).rgb;
}
