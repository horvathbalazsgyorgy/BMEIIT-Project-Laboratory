#ifndef PROJECTLABORATORY_MODEL_H
#define PROJECTLABORATORY_MODEL_H

#include <string>
#include <vector>
#include "../uniform/uniformsource.h"
#include "glm/glm.hpp"

namespace Framework {
    class ShaderProgram;
    class Uniform;
    class Mesh;
    class Material;

    class Model : public UniformSource {
        glm::vec3 position     = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale        = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::mat4 modelMatrix  = glm::mat4(1.0f);
        glm::mat3 normalMatrix = glm::mat3(1.0f);

        void initDump() override;
    protected:
        ShaderProgram* program;
        std::vector<Mesh*> meshes;
        std::vector<Material*> materials;
    public:
        Model(ShaderProgram* program,
            glm::vec3 position = glm::vec3(0.0f),
            glm::vec3 scale = glm::vec3(1.0f),
            const std::string& prefix = "model");

        Model(ShaderProgram* program,
            Mesh* mesh,
            Material* material,
            glm::vec3 position,
            glm::vec3 scale,
            const std::string& prefix = "model")
        : UniformSource(prefix, {program}), position(position), scale(scale), program(program), meshes{mesh}, materials{material}
        {
            Model::initDump();
        }

        Model(ShaderProgram* program,
            std::vector<Mesh*> meshes,
            std::vector<Material*> materials,
            const std::string& prefix = "model")
        : UniformSource(prefix, {program}), program(program), meshes(std::move(meshes)), materials(std::move(materials))
        {
            Model::initDump();
        }

        void setPosition(glm::vec3 pos) { this->position = pos; }
        void setScale(glm::vec3 scaling) { this->scale = scaling; }

        void update();
        virtual void draw();
        ~Model() override = default;
    };
}

#endif //PROJECTLABORATORY_MODEL_H