#version 430 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoord;

uniform struct{
    mat4 modelMatrix;
} model;

uniform struct{
    mat4 viewProjMatrix;
} camera;

out vec2 tex;

void main(void) {
    gl_Position = camera.viewProjMatrix * model.modelMatrix * vec4(vertexPosition, 1.0f);
    tex = vertexTexCoord;
}
