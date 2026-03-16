#ifndef PROJECTLABORATORY_CAMERA_H
#define PROJECTLABORATORY_CAMERA_H

#include <set>
#include <string>
#include <vector>
#include "../opengl/application.h"
#include "../uniform/uniformsource.h"
#include "glm/glm.hpp"

namespace Framework {
    class ShaderProgram;
    class Uniform;

    class Camera : public UniformSource {
        std::vector<ShaderProgram*> programs;
        glm::vec3 position;
        float roll = 0.0f, pitch = 0.0f, yaw = -90.0f;
        float fov = glm::radians(45.0f),
            aspect = (float)WindowSize::width/(float)WindowSize::height,
            nearPlane = 0.1f,
            farPlane = 1000.0f;
        float speed = 10.0f;
        float sensitivity = 0.1f;

        glm::mat4 view = glm::mat4(1.0f), projection = glm::mat4(1.0f), viewProjection, rayDir;
        glm::vec3 ahead = glm::vec3(0.0f, 0.0f, -1.0f),
             right = glm::vec3(1.0f, 0.0f, 0.0f),
             vup = glm::vec3(0.0f, 1.0f, 0.0f);

        Uniform* searchUniform(const std::string& name) const;
    public:
        Camera(std::vector<ShaderProgram*> programs, glm::vec3 position, const std::string& prefix = "camera")
            : UniformSource(prefix), programs(std::move(programs)), position(position) {update();
        }
        Camera(std::vector<ShaderProgram*> programs, glm::vec3 position, float roll, float pitch, float yaw, const std::string& prefix = "camera")
            : UniformSource(prefix), programs(std::move(programs)), position(position), roll(roll), pitch(pitch), yaw(yaw) {
            update();
        }

        void update();
        void move(float dt, const std::set<unsigned int> &keysPressed);
        Uniform* operator[](const std::string& name) const override;

        glm::mat4 getViewProjMatrix() const {
            return viewProjection;
        }
        glm::mat4 getRayDirMatrix() const {
            return rayDir;
        }
        void setPosition(glm::vec3 pos) {
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

        ~Camera() override = default;
    };
}

#endif //PROJECTLABORATORY_CAMERA_H