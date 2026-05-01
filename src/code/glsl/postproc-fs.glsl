#version 430 core

precision highp float;

in vec2 tex;

uniform struct{
    float exposure;
    sampler2D rawTexture;
} material;

out vec4 fragmentColor;
const float gamma = 2.2;

//Post-processing - Apply tone-mapping and gamma correction
void main(void) {
    vec3 color = texture(material.rawTexture, vec2(tex.x, 1.0f - tex.y)).rgb;
    vec3 toneMapped = vec3(1.0f) - exp(-color * material.exposure);
    vec3 finalColor = pow(color, vec3(1.0f/gamma));

    fragmentColor = vec4(finalColor, 1.0f);
}
