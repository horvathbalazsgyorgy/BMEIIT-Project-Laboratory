#include "material.h"

#include <stdexcept>
#include "shaderprogram.h"

namespace Framework {
    Material::Material(ShaderProgram* program, const std::string& prefix) : UniformSource(prefix), program(program) {
        program->useShaderProgram();
    }

    void Material::draw() {
        program->useShaderProgram();
    }

    Uniform* Material::operator[](const std::string& name) const {
        std::string glslUniform = glslPrefix + '.' + name;
        if (auto uniform = program->queryUniform(glslUniform))
            return uniform;
        throw std::runtime_error("Uniform \"" + glslUniform + "\" not found or is not in use.");
    }
}