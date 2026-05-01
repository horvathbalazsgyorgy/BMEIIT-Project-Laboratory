#version 430 core

precision highp float;

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 tex;

out vec2 fragmentColor;
const float PI = 3.14159265359;
const uint nSamples = 4096u;

//Geometry function - Using the Schlick-GGX approximation
float Geometry(float k, float cosa, float cosb){
    float vGGX = cosb / (cosb * (1.0f - k) + k);
    float lGGX = cosa / (cosa * (1.0f - k) + k);

    return vGGX * lGGX;
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

vec2 convoluteBRDF(float cosb, float roughness){
    vec3 viewDir = vec3(sqrt(1.0f - pow(cosb, 2.0)), 0.0f, cosb);
    vec3 normal = normalize(worldNormal);

    vec3 up = abs(normal.z) < 0.999 ? vec3(0.0f, 0.0f, 1.0f) : vec3(1.0f, 0.0f, 0.0f);
    vec3 tangent = normalize(cross(up, normal));
    vec3 bitangent = cross(normal, tangent);

    float k = pow(roughness, 2.0) / 2.0f;

    vec2 lutValue;
    for(uint i = 0; i < nSamples; ++i){
        vec2 sequenceValue = hammersley(i, nSamples);
        vec3 halfway = importanceSampling(sequenceValue, normal, tangent, bitangent, roughness);
        vec3 lightDir = normalize(2.0f * dot(halfway, viewDir) * halfway - viewDir);

        float cosA = max(lightDir.z, 0.0);
        float cosB = max(dot(normal, viewDir),  0.0f);
        float cost = max(halfway.z,  0.0);
        float cos0 = max(dot(halfway, viewDir), 0.0);

        float cosa = max(dot(normal, lightDir), 0.0f);

        if(cosA > 0.0f){
            float G = Geometry(k, cosa, cosB);
            float visiblity = (G * cos0) / (cost * cosb);
            float F = pow(1.0f - cos0, 5.0);

            lutValue.x += (1.0f - F) * visiblity;
            lutValue.y += F * visiblity;
        }
    }
    lutValue /= float(nSamples);
    return lutValue;
}

void main(void) {
    fragmentColor = convoluteBRDF(tex.x, tex.y);
}
