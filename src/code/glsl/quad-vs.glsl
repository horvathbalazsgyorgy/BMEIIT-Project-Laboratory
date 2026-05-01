#version 430 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoord;

out vec3 worldPosition;
out vec3 worldNormal;
out vec2 tex;

void main(void) {
    gl_Position = vec4(vertexPosition, 1.0f);
    worldPosition = vertexPosition;
    worldNormal = vertexNormal;
    tex = vertexTexCoord;
}
