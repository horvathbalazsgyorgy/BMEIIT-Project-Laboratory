#version 430 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoord;

uniform struct {
    mat4 view;
    mat4 projection;
    mat4 invView;
    mat4 invProjection;
    vec3 position;
} camera;

out vec4 viewPosition;
out vec3 worldPosition;
out vec3 worldNormal;
out vec2 tex;

void main(void) {
    gl_Position    = vec4(vertexPosition, 1.0f);
    viewPosition   = camera.invProjection * vec4(vertexPosition.xy, 1.0f, 1.0f);
    viewPosition  /= viewPosition.w;
    worldPosition  = vertexPosition;
    worldNormal    = vertexNormal;
    tex = vertexTexCoord;
}
