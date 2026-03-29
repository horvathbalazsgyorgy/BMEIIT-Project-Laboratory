#include "camera.h"

#include "../utility/mouse.h"
#include "glm/gtc/matrix_transform.hpp"

namespace Framework {
    void Camera::initDump() {
        dump.variables[glslPrefix + ".viewProjMatrix"] = &viewProjection;
        dump.variables[glslPrefix + ".rayDirMatrix"]   = &rayDir;
        dump.variables[glslPrefix + ".position"]       = &position;
    }

    void Camera::update() {
        if (WindowSize::width == 0 || WindowSize::height == 0) {
            return;
        }

        aspect = (float)WindowSize::width/(float)WindowSize::height;
        glm::mat4 view = glm::lookAt(position, position + ahead, vup);
        glm::mat4 projection = glm::perspective(fov, aspect, nearPlane, farPlane);
        viewProjection = projection * view;
        rayDir = inverse(translate(viewProjection, position));
    }

    void Camera::move(const float dt, const std::set<unsigned int> &keysPressed) {
        if (mouse.isMouseDown()) {
            glm::vec2 delta = mouse.getMouseDelta();
            yaw += delta.x * sensitivity;
            pitch += delta.y * sensitivity;

            if (pitch > 89.9f) {
                pitch = 89.9f;
            }
            if (pitch < -89.9f) {
                pitch = -89.9f;
            }
        }

        if (keysPressed.contains('w')) {
            position += ahead * (dt * speed);
        }
        if (keysPressed.contains('s')) {
            position -= ahead * (dt * speed);
        }
        if (keysPressed.contains('a')) {
            position -= right * (dt * speed);
        }
        if (keysPressed.contains('d')) {
            position += right * (dt * speed);
        }
        if (keysPressed.contains('q')) {
            position -= vup * (dt * speed);
        }
        if (keysPressed.contains('e')) {
            position += vup * (dt * speed);
        }

        glm::vec3 dir;
        dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        dir.y = sin(glm::radians(pitch));
        dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        ahead = glm::normalize(dir);
        right = glm::normalize(glm::cross(dir, glm::vec3(0.0f, 1.0f, 0.0f)));
        vup = glm::normalize(glm::cross(right, ahead));

        update();
    }
}
