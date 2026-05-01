#version 430 core

precision highp float;

in vec4 rayDir;

uniform struct{
    samplerCube envTexture;
} material;

out vec4 fragmentColor;
const float PI = 3.14159265359;

void main(void) {
    vec3 normal = normalize(rayDir.xyz);
    vec3 irradiance = vec3(0.0f);

    vec3 worldUp = vec3(0.0f, 1.0f, 0.0f);
    vec3 right   = normalize(cross(worldUp, normal));
    vec3 up      = cross(normal, right);

    float sampleDelta = 0.025f;
    float numSamples  = 0.0f;
    for(float azimuth = 0.0f; azimuth < 2.0f * PI; azimuth += sampleDelta){
        for(float zenith = 0.0f; zenith < 0.5f * PI; zenith += sampleDelta){
            vec3 tangent = vec3(0.0f);
            tangent.x = sin(zenith) * cos(azimuth);
            tangent.y = sin(zenith) * sin(azimuth);
            tangent.z = cos(zenith);

            vec3 sampleVector = tangent.x * right + tangent.y * up + tangent.z * normal;

            irradiance += texture(material.envTexture, sampleVector).rgb * cos(zenith) * sin(zenith);
            numSamples += 1.0f;
        }
    }

    irradiance = PI * irradiance * (1.0f / numSamples);
    fragmentColor = vec4(irradiance, 1.0f);
}
