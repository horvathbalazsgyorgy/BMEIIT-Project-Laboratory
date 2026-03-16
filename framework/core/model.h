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
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::mat4 modelMatrix;
    protected:
        ShaderProgram* program;
        std::vector<Mesh*> meshes;
        std::vector<Material*> materials;
    public:
        Model(ShaderProgram* program, const std::string& prefix = "model");

        Model(ShaderProgram* program, Mesh* mesh, Material* material, glm::vec3 initPosition, const std::string& prefix = "model") :
            UniformSource(prefix), position(initPosition), program(program), meshes{mesh}, materials{material} { }

        Model(ShaderProgram* program, std::vector<Mesh*> meshes, std::vector<Material*> materials, const std::string& prefix = "model") :
            UniformSource(prefix), program(program), meshes(std::move(meshes)), materials(std::move(materials)) { }

        glm::mat4 getModelMatrix() { return modelMatrix; }

        void update();
        virtual void draw();
        Uniform* operator[](const std::string& name) const override;
        ~Model() override = default;
    };
}

#endif //PROJECTLABORATORY_MODEL_H