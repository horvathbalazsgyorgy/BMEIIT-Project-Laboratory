#version 430 core

layout (location = 0)  in vec3  vertexPosition;
layout (location = 1)  in vec4  vertexColor;
layout (location = 2)  in vec2  vertexTexCoords[4];
layout (location = 6)  in vec3  vertexNormal;
layout (location = 7)  in vec3  vertexTangent;
layout (location = 8)  in vec3  vertexBitangent;
layout (location = 9)  in ivec4 blendIndices;
layout (location = 10) in vec4  blendWeights;

uniform struct{
    mat4 modelMatrix;
    mat3 normalMatrix;
    samplerBuffer boneBuffer;
} model;

uniform struct{
    mat4 viewProjMatrix;
    vec3 position;
} camera;

out vec4 worldPosition;
out vec3 worldNormal;
out vec3 worldTangent;
out vec3 worldBitangent;
out vec3 viewDir;
out vec4 color;
out vec2 tex[4];

mat4 getMatrixFromTexture(int blendIndex){
    vec4 c0 = texelFetch(model.boneBuffer, blendIndex * 4 + 0);
    vec4 c1 = texelFetch(model.boneBuffer, blendIndex * 4 + 1);
    vec4 c2 = texelFetch(model.boneBuffer, blendIndex * 4 + 2);
    vec4 c3 = texelFetch(model.boneBuffer, blendIndex * 4 + 3);
    return mat4(c0, c1, c2, c3);
}

void main(void) {
    mat4 m0 = getMatrixFromTexture(blendIndices.x);
    mat4 m1 = getMatrixFromTexture(blendIndices.y);
    mat4 m2 = getMatrixFromTexture(blendIndices.z);
    mat4 m3 = getMatrixFromTexture(blendIndices.w);

    vec4 w = blendWeights;
    mat4 blendedMat    = m0 * w.x + m1 * w.y + m2 * w.z + m3 * w.w;
    mat3 blendedNormal = transpose(inverse(mat3(blendedMat)));

    vec4 blendedPos = blendedMat * vec4(vertexPosition, 1.0f);
    worldPosition   = model.modelMatrix  * blendedPos;
    worldNormal     = model.normalMatrix * blendedNormal * vertexNormal;
    worldTangent    = model.normalMatrix * blendedNormal * vertexTangent;
    worldBitangent  = model.normalMatrix * blendedNormal * vertexBitangent;

    gl_Position = camera.viewProjMatrix * worldPosition;

    viewDir = normalize(camera.position - vec3(worldPosition.xyz));
    for(int i = 0; i < 4; i++) tex[i] = vertexTexCoords[i];
    color   = vertexColor;
}
