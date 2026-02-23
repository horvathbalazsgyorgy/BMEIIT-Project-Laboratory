#ifndef PROJECTLABORATORY_MATERIAL_H
#define PROJECTLABORATORY_MATERIAL_H

#include "shaderprogram.h"
#include "glm/vec3.hpp"

using namespace glm;

namespace Framework {
    class Material {
        vec3 kd;
        ShaderProgram* shaderProgram;
    public:
        Material(ShaderProgram* program, vec3 kd) :
            kd(kd),
            shaderProgram(program) {}

        void set(const char* name, vec3 color) {
            shaderProgram->setUniform(name, color);
        }

        void draw() {
            shaderProgram->useShaderProgram();
        }
    };
}

#endif //PROJECTLABORATORY_MATERIAL_H