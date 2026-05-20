#version 430 core

precision highp float;

in vec4 worldPosition;
in vec3 worldNormal;
in vec3 worldTangent;
in vec3 worldBitangent;
in vec4 color;
in vec2 tex[4];

uniform struct{
    sampler2D albedoMap1;
    sampler2D normalMap1;
    sampler2D mrMap1;
    sampler2D aoMap1;

    int albedoMap1_uv;
    int normalMap1_uv;
    int mrMap1_uv;
    int aoMap1_uv;
} material;

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 albedo;
layout (location = 3) out vec3 pbr;

void main(void) {
    position = worldPosition.xyz;

    vec3 N = gl_FrontFacing ? normalize(worldNormal) : -normalize(worldNormal);
    vec3 T = normalize(worldTangent);
    vec3 B = normalize(worldBitangent);
    mat3 TBN = mat3(T, B, N);
    vec3 tangentNormal = texture(material.normalMap1, tex[material.normalMap1_uv]).rgb;
    tangentNormal = normalize(tangentNormal * 2.0 - 1.0);
    normal        = normalize(TBN * tangentNormal);

    vec4 materialColor = texture(material.albedoMap1, tex[material.albedoMap1_uv]);
    if(materialColor.a < 0.1){
        discard;
    }
    albedo = materialColor.rgb * color.rgb;

    vec3  metallicRougness = texture(material.mrMap1, tex[material.mrMap1_uv]).rgb;
    float ao = texture(material.aoMap1, tex[material.aoMap1_uv]).r;
    pbr.r = ao;
    pbr.g = metallicRougness.g;
    pbr.b = metallicRougness.b;
}
