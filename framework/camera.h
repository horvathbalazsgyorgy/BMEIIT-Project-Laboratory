#ifndef PROJECTLABORATORY_CAMERA_H
#define PROJECTLABORATORY_CAMERA_H

#include <set>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace glm;

//NOTE: Prone to changes
class Camera {
    vec3 position;
    float roll = 0.0f, pitch = 0.0f, yaw = 0.0f;
    float fov = 1.0f, aspect = 1.0f, nearPlane = 1.0f, farPlane = 500.0f;
    mat4 view, projection, viewProjection, rayDir;

    vec3 ahead = vec3(0.0f, 0.0f, -1.0f),
         right = vec3(1.0f, 0.0f, 0.0f),
         vup = vec3(0.0f, 0.0f, 1.0f);

    float speed = 10.0f;
    bool dragging = false, mouseDown = false;
public:
    Camera(vec3 position) : position(position) { }
    Camera(vec3 position, float roll, float pitch, float yaw)
        : position(position), roll(roll), pitch(pitch), yaw(yaw) { }

    void update() {
        vec3 direction = vec3();
        direction.x = cos(radians(yaw)) * cos(radians(pitch));
        direction.y = sin(radians(pitch));
        direction.z = sin(radians(yaw)) * cos(radians(pitch));
        ahead = normalize(direction);
        right = normalize(cross(ahead, vup));
        vup = normalize(cross(right, ahead));

        view = lookAt(position, position + ahead, vup);
        projection = perspective(fov, aspect, nearPlane, farPlane);
        viewProjection = projection * view;

        rayDir = inverse(translate(mat4(), position) * viewProjection);
    }

    void setPosition(vec3 pos) {
        this->position = pos;
    }
    void setRotation(float r = 0.0f, float p = 0.0f, float y = 0.0f) {
        roll = r;
        pitch = p;
        yaw = y;
    }
    void setSpeed(float s) {
        speed = s;
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

    //TODO: Look around with mouse
    void move(float dt, std::set<unsigned int> keysPressed) {
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

        update();
    }
};

#endif //PROJECTLABORATORY_CAMERA_H