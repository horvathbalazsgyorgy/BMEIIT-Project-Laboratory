#include "model.h"

#include <stdexcept>
#include "material.h"
#include "mesh.h"
#include "../opengl/shaderprogram.h"
#include "glm/gtc/matrix_transform.hpp"

namespace Framework {
    void Model::initDump() {
        dump.variables[glslPrefix + ".modelMatrix"] = &modelMatrix;
        dump.variables[glslPrefix + ".position"] = &position;
    }

    Model::Model(ShaderProgram* program, glm::vec3 position, glm::vec3 scale, const std::string& prefix)
    : UniformSource(prefix, {program}), program(program)
    {
        this->position = position;
        this->scale = scale;
        Model::initDump();
        meshes = std::vector<Mesh*>();
        materials = std::vector<Material*>();
    }

    void Model::update() {
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::scale(modelMatrix, scale);
        modelMatrix = glm::translate(modelMatrix, position);
    }
    
    void Model::draw() {
        program->useShaderProgram(this);
        for (auto mesh : meshes) {
            mesh->draw();
        }
    }
}
