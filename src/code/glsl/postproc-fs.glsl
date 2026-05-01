#version 430 core

precision highp float;

in vec2 tex;

uniform struct{
    sampler2D rawTexture;
} material;

out vec4 fragmentColor;
const float gamma = 2.2;

//Post-processing - Apply gamma correction
void main(void) {
    vec3 color = texture(material.rawTexture, vec2(tex.x, 1.0f - tex.y)).rgb;
    fragmentColor = vec4(pow(color, vec3(1.0f/gamma)), 1.0f);
}
