#ifndef PROJECTLABORATORY_MATERIAL_H
#define PROJECTLABORATORY_MATERIAL_H

#include "shaderprogram.h"

using namespace glm;

namespace Framework {
    //NOTE: Prone to changes
    //TODO: Custom glsl struct name
    class Material {
        ShaderProgram* shaderProgram;
    public:
        Material(ShaderProgram* program) : shaderProgram(program) { }

        void draw() {
            shaderProgram->useShaderProgram();
        }

        Uniform* operator[](const string& name) {
            string prefix = "material.";
            prefix.append(name);
            return shaderProgram->queryUniform(prefix);
        }
    };
}

#endif //PROJECTLABORATORY_MATERIAL_H