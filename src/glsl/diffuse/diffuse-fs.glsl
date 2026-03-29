#version 430 core

precision highp float;

in vec2 tex;

uniform struct{
    sampler2D diffuseMap1;
}material;

out vec4 fragmentColor;

void main(void) {
    fragmentColor = texture(material.diffuseMap1, tex.xy);
}
