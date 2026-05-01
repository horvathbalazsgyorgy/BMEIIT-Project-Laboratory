#version 430 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoord;
layout (location = 3) in vec3 vertexTangent;
layout (location = 4) in vec3 vertexBitangent;

uniform struct{
    mat4 modelMatrix;
    mat3 normalMatrix;
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
out vec2 tex;

void main(void) {
    worldPosition  = model.modelMatrix * vec4(vertexPosition, 1.0f);
    worldNormal    = model.normalMatrix * vertexNormal;
    worldTangent   = model.normalMatrix * vertexTangent;
    worldBitangent = model.normalMatrix * vertexBitangent;

    gl_Position    = camera.viewProjMatrix * worldPosition;

    viewDir = normalize(camera.position - vec3(worldPosition.xyz));
    tex = vertexTexCoord;
}
