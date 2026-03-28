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
        glm::vec3 position;
        float pitch       =   0.0f,
              yaw         = -90.0f,
              speed       =  10.0f,
              sensitivity =   0.1f;

        float fov       = glm::radians(45.0f),
              aspect    = (float)WindowSize::width/(float)WindowSize::height,
              nearPlane = 0.1f,
              farPlane  = 1000.0f;

        glm::mat4 viewProjection = glm::mat4(1.0f),
                  rayDir         = glm::mat4(1.0f);

        glm::vec3 ahead = glm::vec3(0.0f, 0.0f, -1.0f),
                  right = glm::vec3(1.0f, 0.0f, 0.0f),
                  vup   = glm::vec3(0.0f, 1.0f, 0.0f);

        void initDump() override;
    public:
        Camera(const std::vector<ShaderProgram*>& programs,
            const glm::vec3 position,
            const std::string& prefix = "camera")
            : UniformSource(prefix, programs), position(position)
        {
            Camera::initDump();
            update();
        }
        Camera(const std::vector<ShaderProgram*>& programs,
            const glm::vec3 position,
            const float pitch,
            const float yaw,
            const std::string& prefix = "camera")
            : UniformSource(prefix, programs), position(position), pitch(pitch), yaw(yaw)
        {
            Camera::initDump();
            update();
        }

        void setPosition(const glm::vec3 pos) {
            this->position = pos;
        }
        void setRotation(const float p, const float y) {
            pitch = p;
            yaw = y;
        }
        void setFov(const float f) {
            fov = f;
        }
        void setAspectRatio(const float aspectRatio) {
            this->aspect = aspectRatio;
        }
        void setPlanes(const float n, const float f) {
            nearPlane = n;
            farPlane = f;
        }
        void setSpeed(const float s) {
            speed = s;
        }
        void setSensitivity(const float sens) {
            sensitivity = sens;
        }

        void update();
        void move(float dt, const std::set<unsigned int> &keysPressed);

        ~Camera() override = default;
    };
}

#endif //PROJECTLABORATORY_CAMERA_H