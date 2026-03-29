#include "uniformsource.h"

#include <stdexcept>
#include <utility>
#include "../opengl/shaderprogram.h"
#include "uniform.h"

namespace Framework {
    UniformSource::UniformSource(std::string prefix, const std::vector<ShaderProgram*>& programs) : glslPrefix(std::move(prefix)) {
        if (programs.empty())
            throw std::invalid_argument("Invalid argument; expected at least one shader program, but none were provided.");
        for (auto program : programs) {
            program->subscribe(this);
        }
    }

    void UniformSource::update(const ShaderProgram* program) {
        if (dump.variables.empty() || dump.uniforms.empty())
            return;

        for (const auto& key : dump.uniforms) {
            if (auto uniform = program->queryUniform(key)) {
                auto it = dump.variables.find(key);
                if (it == dump.variables.end())
                    throw std::runtime_error("Invalid dump entry; valid uniform "
                                             "\"" + key + "\" not found in dump for auto binding.");

                std::visit([uniform](auto&& value)
                {
                    using T = std::decay_t<decltype(value)>;
                    if constexpr (std::is_same_v<T, Texture*>) {
                        uniform->set(value);
                    }else {
                        uniform->set(*value);
                    }
                }, it->second);
            }
        }
    }

    UniformSource& UniformSource::operator+=(const std::string& name) {
        dump.uniforms.push_back(glslPrefix + '.' + name);
        return *this;
    }

    Uniform* UniformSource::operator()(const std::string& name, const ShaderProgram* program) const {
        const std::string glslUniform = glslPrefix + '.' + name;
        program->useShaderProgram();
        if (auto uniform = program->queryUniform(glslUniform))
            return uniform;
        throw std::runtime_error("Uniform \"" + glslUniform + "\" not found or is not in use.");
    }
}
