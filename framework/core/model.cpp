#include "model.h"

#include <stdexcept>
#include "material.h"
#include "mesh.h"
#include "../opengl/shaderprogram.h"
#include "glm/gtc/matrix_transform.hpp"

namespace Framework {
    Model::Model(ShaderProgram* program, const std::string& prefix) : UniformSource(prefix), program(program) {
        meshes = std::vector<Mesh*>();
        materials = std::vector<Material*>();
    }

    void Model::update() {
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::scale(modelMatrix, scale);
        modelMatrix = glm::translate(modelMatrix, position);
    }

    void Model::draw() {
        for (auto material : materials) {
            material->draw();
        }
        for (auto mesh : meshes) {
            mesh->draw();
        }
    }

    Uniform* Model::operator[](const std::string& name) const {
        std::string glslUniform = glslPrefix + '.' + name;
        if (auto uniform = program->queryUniform(glslUniform))
            return uniform;
        throw std::runtime_error("Uniform \"" + glslUniform + "\" not found or is not in use.");
    }
}
