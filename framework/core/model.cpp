#include "model.h"

#include <stdexcept>
#include "material.h"
#include "mesh.h"
#include "glm/gtc/matrix_transform.hpp"

namespace Framework {
    void Model::initDump() {
        this->linkUniform("modelMatrix", &modelMatrix);
        this->linkUniform("normalMatrix", &normalMatrix);
        this->linkUniform("position", &position);
    }

    Model::Model(ShaderProgram* program, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, const std::string& prefix)
    : UniformSource(prefix + '.', {program}), position(position), scale(scale), rotation(rotation)
    {
        program->subscribe(this);
        Model::initDump();
        meshes = std::vector<Mesh*>();
    }

    void Model::update() {
        modelMatrix  = glm::mat4(1.0f);
        modelMatrix  = glm::translate(modelMatrix, position);
        modelMatrix  = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix  = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix  = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix  = glm::scale(modelMatrix, scale);

        normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
    }

    void Model::draw(const ShaderProgram *provider) {
        update();
        UniformSource::draw(provider);
        for (auto* mesh : meshes) {
            mesh->draw(provider);
        }
    }

    void Model::relink(const std::vector<ShaderProgram *> &programs) {
        UniformSource::relink(programs);
        for (auto* program : programs) {
            program->subscribe(this);
        }

        for (auto* mesh : meshes) {
            mesh->relink(programs);
        }
    }
}
