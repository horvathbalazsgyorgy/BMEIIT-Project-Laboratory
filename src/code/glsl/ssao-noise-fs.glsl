#version 430 core

precision highp float;

in vec4 viewPosition;
in vec2 tex;

uniform struct{
    int kernelSize;
    int resolution;
    sampler2D gNormal;
    sampler2D noise;
    samplerBuffer kernel;
} material;

uniform struct {
    mat4 view;
    mat4 projection;
    mat4 invView;
    mat4 invProjection;
    vec3 position;
} camera;

uniform int windowWidth;
uniform int windowHeight;

const float radius = 0.3f;
const float bias = 0.02f;
const float constrast = 3.0f;

layout (location = 0) out float fragmentColor;

void main(void) {
    const float res = material.resolution;
    const vec2 noiseScale = vec2(float(windowWidth) / res, float(windowHeight) / res);

    vec3  viewRay  = viewPosition.xyz;
    float depth    = texture(material.gNormal, tex).a;
    vec3  position = viewRay * depth;
    vec3 normal    = normalize(texture(material.gNormal, tex).rgb);
    normal         = normalize(mat3(camera.view) * normal);

    vec3 randomSample = normalize(texture(material.noise, tex * noiseScale).rgb);

    vec3 tangent = normalize(randomSample - normal * dot(randomSample, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0f;
    for(int i = 0; i < material.kernelSize; ++i){
        vec3 samplePos = TBN * texelFetch(material.kernel, i).xyz;
        samplePos = position + samplePos * radius;

        vec4 offset = vec4(samplePos, 1.0f);
        offset = camera.projection * offset;
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5f + 0.5f;

        float sampleDepth = (viewRay * texture(material.gNormal, offset.xy).a).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius/abs(position.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0f : 0.0f) * rangeCheck;
    }

    occlusion = 1.0f - (occlusion / material.kernelSize);
    fragmentColor = pow(occlusion, constrast);
}
