#version 430 core

precision highp float;

in vec2 tex;

uniform struct{
    int bloom;
    float exposure;
    sampler2D rawTexture;
    sampler2D bloomTexture;
} material;

uniform struct{
    float fade;
    int cinematic;
} camera;

out vec4 fragmentColor;
const float gamma = 2.2;

//Courtesy of Krzysztof Narkowicz
//https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 ACEScurve(vec3 x){
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp(vec3((x * (a*x+b)) / (x * (c*x+d) + e)), 0.0, 1.0);
}

//Post-processing
void main(void) {
    vec3 hdrColor = texture(material.rawTexture, tex).rgb;

    if(material.bloom == 1){
        vec3 bloomColor = texture(material.bloomTexture, tex).rgb;
        hdrColor = mix(hdrColor, bloomColor, 0.2f);
    }

    vec3 color = hdrColor * material.exposure;
    vec3 toneMapped = ACEScurve(color);

    fragmentColor = vec4(pow(toneMapped, vec3(1.0f/gamma)), 1.0f);
    fragmentColor *= camera.fade;

    if(camera.cinematic == 1){
        if(tex.y <= 0.1f || tex.y >= 0.9f){
            fragmentColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
}
