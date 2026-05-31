#version 430 core

precision highp float;

in vec4 viewPosition;
in vec3 worldNormal;
in vec3 worldTangent;
in vec3 worldBitangent;
in vec4 color;
in vec2 tex[4];

uniform struct{
    sampler2D albedoMap1;
    sampler2D normalMap1;
    sampler2D emissiveMap1;
    sampler2D mrMap1;

    int albedoMap1_uv;
    int normalMap1_uv;
    int emissiveMap1_uv;
    int mrMap1_uv;

    vec3 emissiveFactor;
} material;

uniform struct{
    mat4 view;
    mat4 projection;
    float farPlane;
} camera;

layout (location = 0) out vec4 normal;
layout (location = 1) out vec3 albedo;
layout (location = 2) out vec2 pbr;
layout (location = 3) out vec3 emission;

void main(void) {
    float depth = -viewPosition.z / camera.farPlane;

    vec3 N = gl_FrontFacing ? normalize(worldNormal) : -normalize(worldNormal);
    vec3 T = normalize(worldTangent);
    vec3 B = normalize(worldBitangent);
    mat3 TBN = mat3(T, B, N);
    vec3 tangentNormal = texture(material.normalMap1, tex[material.normalMap1_uv]).rgb;
    tangentNormal = normalize(tangentNormal * 2.0 - 1.0);
    normal.xyz    = normalize(TBN * tangentNormal);
    normal.w      = depth;

    vec4 materialColor = texture(material.albedoMap1, tex[material.albedoMap1_uv]);
    if(materialColor.a < 0.1){
        discard;
    }
    albedo.xyz = materialColor.rgb * color.rgb;

    vec3 metallicRougness = texture(material.mrMap1, tex[material.mrMap1_uv]).rgb;
    pbr.x = metallicRougness.g;
    pbr.y = metallicRougness.b;

    vec3 emissive = texture(material.emissiveMap1, tex[material.emissiveMap1_uv]).rgb;
    emissive     *= material.emissiveFactor;
    emission.xyz  = emissive;
}
