#ifndef PROJECTLABORATORY_CAMERA_H
#define PROJECTLABORATORY_CAMERA_H

#include <set>
#include "mouse.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace glm;

//NOTE: Prone to changes
class Camera {
    vec3 position;
    float roll = 0.0f, pitch = 0.0f, yaw = 0.0f;
    float fov = 1.0f, aspect = 1.0f, nearPlane = 1.0f, farPlane = 500.0f;
    float speed = 10.0f;
    float sensitivity = 0.005f;

    mat4 rotation, view, projection, viewProjection, rayDir;
    vec3 ahead = vec3(0.0f, 0.0f, -1.0f),
         right = vec3(1.0f, 0.0f, 0.0f),
         vup = vec3(0.0f, 1.0f, 0.0f);

public:
    Camera(vec3 position) : position(position) { }
    Camera(vec3 position, float roll, float pitch, float yaw)
        : position(position), roll(roll), pitch(pitch), yaw(yaw) { }

    void updateTransformation() {
        rotation = rotate(rotate(rotate(mat4(),
            roll, vec3(0.0f, 0.0f, -1.0f)),
            pitch, vec3(1.0f, 0.0f, 0.0f)),
            yaw, vec3(0.0f, 1.0f, 0.0f));

        view = lookAt(position, position + ahead, vup);
        projection = perspective(fov, aspect, nearPlane, farPlane);
        viewProjection = projection * view;

        rayDir = inverse(translate(mat4(), position) * viewProjection);
    }

    void updateCoordinateSystem() {
        ahead = xyz(xyz0(vec3(0.0f, 0.0f, -1.0f)) * rotation);
        right = xyz(xyz0(vec3(1.0f, 0.0f, 0.0f)) * rotation);
        vup = xyz(xyz0(vec3(0.0f, 1.0f, 0.0f)) * rotation);
    }

    void setPosition(vec3 pos) {
        this->position = pos;
    }
    void setRotation(float r = 0.0f, float p = 0.0f, float y = 0.0f) {
        roll = r;
        pitch = p;
        yaw = y;
    }
    void setFov(float f) {
        fov = f;
    }
    void setAspectRatio(float aspectRatio) {
        this->aspect = aspectRatio;
    }
    void setPlanes(float n, float f) {
        nearPlane = n;
        farPlane = f;
    }
    void setSpeed(float s) {
        speed = s;
    }
    void setSensitivity(float s) {
        sensitivity = s;
    }

    void move(float dt, std::set<unsigned int> keysPressed) {
        if (mouse.isMouseDown()) {
            yaw += mouse.getCursorPos().x * sensitivity;
            pitch += mouse.getCursorPos().y * sensitivity;

            if (pitch> 3.14f/2.0f) {
                pitch = 3.14f/2.0f;
            }
            if (pitch < -3.14f/2.0f) {
                pitch = -3.14f/2.0f;
            }
        }

        if (keysPressed.find('w') != keysPressed.end()) {
            position += ahead * (dt * speed);
        }
        if (keysPressed.find('s') != keysPressed.end()) {
            position -= ahead * (dt * speed);
        }
        if (keysPressed.find('a') != keysPressed.end()) {
            position -= right * (dt * speed);
        }
        if (keysPressed.find('d') != keysPressed.end()) {
            position += right * (dt * speed);
        }
        if (keysPressed.find('q') != keysPressed.end()) {
            position -= vup * (dt * speed);
        }
        if (keysPressed.find('e') != keysPressed.end()) {
            position += vup * (dt * speed);
        }

        updateTransformation();
        updateCoordinateSystem();
    }
};

#endif //PROJECTLABORATORY_CAMERA_H