#ifndef PROJECTLABORATORY_MODEL_H
#define PROJECTLABORATORY_MODEL_H

#include "material.h"
#include "mesh.h"
#include "uniform.h"
#include "glm/glm.hpp"

using namespace glm;

namespace Framework {
    class Model {
        Mesh* mesh;
        Material* material;
        ShaderProgram* program;

        vec3 position;
        vec3 scale = vec3(1.0f, 1.0f, 1.0f);
        mat4 modelMatrix;
    public:
        Model(ShaderProgram* program, Mesh* mesh, Material* material, vec3 initialPos) :
            program(program), mesh(mesh), material(material), position(initialPos) { }

        mat4 getModelMatrix() {
            return modelMatrix;
        }

        void update() {
            modelMatrix = mat4(1.0f);
            modelMatrix = glm::scale(modelMatrix, scale);
            modelMatrix = translate(modelMatrix, position);
        }

        void draw() {
            material->draw();
            mesh->draw();
        }

        Uniform* operator[](const string& name) {
            string prefix = "model.";
            prefix.append(name);
            return program->queryUniform(prefix);
        }
    };
}

#endif //PROJECTLABORATORY_MODEL_H