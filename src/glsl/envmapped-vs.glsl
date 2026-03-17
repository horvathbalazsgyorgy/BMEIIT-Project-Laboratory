#version 430 core

layout (location = 0) in vec3 vertexPosition;

uniform struct {
    mat4 rayDirMatrix;
    vec3 kd;
} camera;

out vec4 rayDir;

void main(void) {
    gl_Position = vec4(vertexPosition.x, vertexPosition.y, 0.99999f, 1.0f);
    rayDir = camera.rayDirMatrix * vec4(vertexPosition, 1.0f);
}
