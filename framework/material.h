#ifndef PROJECTLABORATORY_MATERIAL_H
#define PROJECTLABORATORY_MATERIAL_H

#include "uniform.h"
#include "shaderprogram.h"

using namespace glm;

namespace Framework {
    //NOTE: Prone to changes
    //TODO: Custom glsl struct name
    class Material {
        ShaderProgram* program;
    public:
        Material(ShaderProgram* program) : program(program) {
            program->useShaderProgram();
        }

        void draw() {
            program->useShaderProgram();
        }

        Uniform* operator[](const string& name) {
            string prefix = "material.";
            prefix.append(name);
            return program->queryUniform(prefix);
        }
    };
}

#endif //PROJECTLABORATORY_MATERIAL_H