#ifndef PROJECTLABORATORY_CAMERA_H
#define PROJECTLABORATORY_CAMERA_H

#include <complex>
#include <set>
#include "mouse.h"
#include "uniform.h"
#include "shaderprogram.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace glm;

namespace Framework {
    //NOTE: Prone to changes
    class Camera {
        ShaderProgram* program;
        vec3 position;
        float roll = 0.0f, pitch = 0.0f, yaw = -90.0f;
        float fov = radians(45.0f), aspect = 1.0f, nearPlane = 0.1f, farPlane = 1000.0f;
        float speed = 10.0f;
        float sensitivity = 0.1f;

        mat4 view = mat4(1.0f), projection = mat4(1.0f), viewProjection, rayDir;
        vec3 ahead = vec3(0.0f, 0.0f, -1.0f),
             right = vec3(1.0f, 0.0f, 0.0f),
             vup = vec3(0.0f, 1.0f, 0.0f);
    public:
        Camera(ShaderProgram* program, vec3 position) : position(position), program(program) {
            update();
        }
        Camera(ShaderProgram* program, vec3 position, float roll, float pitch, float yaw)
            : program(program), position(position), roll(roll), pitch(pitch), yaw(yaw) {
            update();
        }

        void update() {
            view = lookAt(position, position + ahead, vup);
            projection = perspective(fov, aspect, nearPlane, farPlane);
            viewProjection = projection * view;

            rayDir = inverse(translate(mat4(1.0f), position) * viewProjection);
        }

        mat4 getView() {
            return lookAt(position, position + ahead, vup);
        }

        mat4 getProjection() {
            return perspective(fov, aspect, nearPlane, farPlane);
        }

        mat4 getRayDirMatrix() {
            return rayDir;
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
                vec2 delta = mouse.getMouseDelta();
                yaw += delta.x * sensitivity;
                pitch += delta.y * sensitivity;

                if (pitch > 89.9f) {
                    pitch = 89.9f;
                }
                if (pitch < -89.9f) {
                    pitch = -89.9f;
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

            vec3 dir;
            dir.x = cos(radians(yaw)) * cos(radians(pitch));
            dir.y = sin(radians(pitch));
            dir.z = sin(radians(yaw)) * cos(radians(pitch));
            ahead = normalize(dir);
            right = normalize(cross(dir, vec3(0.0f, 1.0f, 0.0f)));
            vup = normalize(cross(right, ahead));

            update();
        }

        Uniform* operator[](const string& name) {
            string prefix = "camera.";
            prefix.append(name);
            return program->queryUniform(prefix);
        }
    };
}

#endif //PROJECTLABORATORY_CAMERA_H