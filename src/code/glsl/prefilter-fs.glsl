#version 430 core

precision highp float;

in vec4 rayDir;

uniform struct {
    samplerCube envTexture;
    float roughness;
} material;

out vec4 fragmentColor;
const float PI = 3.14159265359;
const uint nSamples = 4096u;

//Normal Distribution function - Using the Trowbridge-Reitz GGX
float Distribution(float roughness, float cost){
    float a     = pow(roughness, 2.0);
    float a2    = pow(a, 2.0);
    float cos0  = pow(cost, 2.0);
    float denom = PI * pow(cos0 * (a2 - 1.0f) + 1.0f, 2.0);

    return a2 / denom;
}

float vanDerCorput(uint bits){
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10f;
}

vec2 hammersley(uint i, uint N){
    return vec2(float(i)/float(N), vanDerCorput(i));
}

vec3 importanceSampling(vec2 sqValue, vec3 normal, vec3 tangent, vec3 bitangent, float roughness){
    float a  = pow(roughness, 2.0);
    float a2 = pow(a, 2.0);

    float azimuth = 2.0f * PI * sqValue.x;
    float cost = sqrt((1.0 - sqValue.y) / (1.0f + (a2 - 1.0f) * sqValue.y));
    float sint = sqrt(1.0 - pow(cost, 2.0));

    vec3 tangentSample;
    tangentSample.x = cos(azimuth) * sint;
    tangentSample.y = sin(azimuth) * sint;
    tangentSample.z = cost;

    vec3 sampleVector = tangentSample.x * tangent + tangentSample.y * bitangent + tangentSample.z * normal;
    return normalize(sampleVector);
}

void main(void) {
    vec3 normal  = normalize(rayDir.xyz);
    vec3 viewDir = normal;

    vec3 up = abs(normal.z) < 0.999 ? vec3(0.0f, 0.0f, 1.0f) : vec3(1.0f, 0.0f, 0.0f);
    vec3 tangent = normalize(cross(up, normal));
    vec3 bitangent = cross(normal, tangent);

    float totalWeight   = 0.0f;
    vec3  prefilterColor = vec3(0.0f);
    for(uint i = 0u; i < nSamples; ++i){
        vec2 sequenceValue = hammersley(i, nSamples);
        vec3 halfway  = importanceSampling(sequenceValue, normal, tangent, bitangent, material.roughness);
        vec3 lightDir = normalize(2.0f * dot(halfway, viewDir) * halfway - viewDir);

        float cosa = max(dot(normal, lightDir), 0.0);
        if(cosa > 0.0f){
            float cost = max(dot(normal, halfway), 0.0);
            float cos0 = max(dot(halfway, viewDir), 0.0);

            float ND = Distribution(material.roughness, cost);
            float densityFuction = (ND * cost / (4.0f * cos0)) + 0.0001;
            float mipTexel  = 4.0f * PI / (6.0f * 1024.0f * 1024.0f);
            float mipSample = 1.0f / (float(nSamples) * densityFuction + 0.0001);
            float mipLevel  = material.roughness == 0.0f ? 0.0f : 0.5f * log2(mipSample / mipTexel);

            prefilterColor += textureLod(material.envTexture, lightDir, mipLevel).rgb * cosa;
            totalWeight    += cosa;
        }
    }

    prefilterColor = prefilterColor / totalWeight;
    fragmentColor  = vec4(prefilterColor, 1.0f);
}
