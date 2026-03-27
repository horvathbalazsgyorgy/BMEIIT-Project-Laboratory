#include "uniformsource.h"

#include <stdexcept>
#include "../opengl/shaderprogram.h"
#include "uniform.h"
#include "uniformregistry.h"

namespace Framework {
    void UniformSource::initSource(const std::vector<ShaderProgram*>& programs) {
        initDump();
        for (auto program : programs) {
            program->subscribe(this);
        }
    }

    UniformSource::UniformSource(const std::string &prefix, const std::vector<ShaderProgram*>& programs) : glslPrefix(prefix) {
        if (programs.empty())
            throw std::invalid_argument("Invalid argument; expected at least one shader program, but none were provided.");
        initSource(programs);
    }

    void UniformSource::update(const ShaderProgram* program) {
        for (const auto& key : dump.uniforms) {
            if (auto uniform = program->queryUniform(key)) {
                auto var = dump.variables[key];
                std::visit([uniform](auto&& value) {
                    uniform->set(*value);
                }, var);
            }
        }
    }

    UniformSource& UniformSource::operator+=(const std::string& name) {
        dump.uniforms.push_back(glslPrefix + '.' + name);
        return *this;
    }

    Uniform* UniformSource::operator()(const std::string& name, const ShaderProgram* program) const {
        const std::string glslUniform = glslPrefix + '.' + name;
        if (auto uniform = program->queryUniform(glslUniform))
            return uniform;
        throw std::runtime_error("Uniform \"" + glslUniform + "\" not found or is not in use.");
    }
}
