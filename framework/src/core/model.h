#ifndef PROJECTLABORATORY_MODEL_H
#define PROJECTLABORATORY_MODEL_H

#include <string>
#include <utility>
#include <vector>
#include "../uniform/uniformsource.h"
#include "glm/glm.hpp"

namespace Framework {
    class ShaderProgram;
    class Uniform;
    class Mesh;
    class Material;

    class Model : public UniformSource {
    protected:
        glm::vec3 position     = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale        = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 rotation     = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::mat4 modelMatrix  = glm::mat4(1.0f);
        glm::mat3 normalMatrix = glm::mat3(1.0f);

        std::vector<Mesh*> meshes;
        void initDump() override;
    public:
        Model(ShaderProgram* program,
            glm::vec3 position = glm::vec3(0.0f),
            glm::vec3 scale    = glm::vec3(1.0f),
            glm::vec3 rotation = glm::vec3(0.0f),
            const std::string& prefix = "model");

        Model(ShaderProgram* program,
            std::vector<Mesh*> meshes,
            glm::vec3 position = glm::vec3(0.0f),
            glm::vec3 scale    = glm::vec3(1.0f),
            glm::vec3 rotation = glm::vec3(0.0f),
            const std::string& prefix = "model")
        : UniformSource(prefix + '.', {program}),
            position(position), scale(scale), rotation(rotation), meshes{std::move(meshes)}
        {
            program->subscribe(this);
            Model::initDump();
        }

        void setPosition(glm::vec3 pos)  { this->position = pos;  }
        void setScale(glm::vec3 scaling) { this->scale = scaling; }
        void setRotation(glm::vec3 rot)  { this->rotation = rot;  }

        void update();
        void draw(const ShaderProgram *provider) override;
        void relink(const std::vector<ShaderProgram *> &programs) override;
        ~Model() override = default;
    };
}

#endif //PROJECTLABORATORY_MODEL_H