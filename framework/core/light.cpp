#include "light.h"

#include <string>
#include <stdexcept>
#include "../opengl/shaderprogram.h"

namespace Framework {
    Uniform* Light::searchUniform(const std::string& name) const {
        for (auto* program : programs) {
            Uniform* uniform = program->queryUniform(name);
            if (uniform) {
                return uniform;
            }
        }
        return nullptr;
    }

    Uniform* Light::operator[](const std::string& name) const {
        std::string glslUniform = glslPrefix + '.' + name;
        if (auto uniform = searchUniform(glslUniform)) {
            return uniform;
        }
        throw std::runtime_error("Uniform \"" + glslUniform + "\" not found or is not in use.");
    }
}
