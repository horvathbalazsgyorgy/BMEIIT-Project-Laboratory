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
    protected:
        glm::vec3 position;
        float pitch       =   0.0f,
              yaw         = -90.0f,
              speed       =  10.0f,
              sensitivity =   0.1f;

        float fov       = glm::radians(45.0f),
              aspect    = (float)WindowSize::width/(float)WindowSize::height,
              nearPlane = 0.1f,
              farPlane  = 1000.0f;

        glm::mat4 view              = glm::mat4(1.0f),
                  projection        = glm::mat4(1.0f),
                  invView           = glm::mat4(1.0f),
                  invProjection     = glm::mat4(1.0f),
                  rayDir            = glm::mat4(1.0f);

        glm::vec3 ahead = glm::vec3(0.0f, 0.0f, -1.0f),
                  right = glm::vec3(1.0f, 0.0f, 0.0f),
                  vup   = glm::vec3(0.0f, 1.0f, 0.0f);

        void initDump() override;
    public:
        Camera(const std::vector<ShaderProgram*>& programs,
            const glm::vec3 position,
            const float pitch = 0.0f,
            const float yaw   = -90.0f,
            const std::string& prefix = "camera")
            : UniformSource(prefix + '.', programs), position(position), pitch(pitch), yaw(yaw)
        {
            for (auto* program : programs) {
                program->subscribe(this);
            }
            Camera::initDump();
            update();
        }

        glm::vec3& Position() { return position; }
        float& Pitch() { return pitch; }
        float& Yaw() { return yaw; }
        float& Speed() { return speed; }
        float& Sensitivity() { return sensitivity; }

        float& Fov() { return fov; }
        float& Aspect() { return aspect; }
        float& NearPlane() { return nearPlane; }
        float& FarPlane() { return farPlane; }

        glm::mat4& View() { return view; }
        glm::mat4& Projection() { return projection; }
        glm::mat4& InvView() { return invView; }
        glm::mat4& InvProjection() { return invProjection; }
        glm::mat4& RayDir() { return rayDir; }

        void update();
        void configureTransformation();
        void configureCoordinateSystem(const glm::vec3& worldUp = glm::vec3(0.0f, 1.0f, 0.0f));
        virtual void move(float dt, const std::set<unsigned int> &keysPressed);
        void relink(const std::vector<ShaderProgram *> &programs) override;

        ~Camera() override = default;
    };
}

#endif //PROJECTLABORATORY_CAMERA_H