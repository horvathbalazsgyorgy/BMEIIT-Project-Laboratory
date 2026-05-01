#include "uniformsource.h"

#include <ranges>
#include <utility>
#include "uniform.h"
#include "../opengl/shaderprogram.h"

namespace Framework {
    void visit(Uniform* uniform, const Dump& var) {
        std::visit([uniform](auto&& value)
        {
            using T = std::decay_t<decltype(value)>;
            DumpVar uniformVar;
            if constexpr (std::is_same_v<T, DumpProvider>) uniformVar = value();
            else uniformVar = value;

            std::visit([uniform](auto&& value) {
                using D = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<D, Texture*>) {
                    uniform->set(value);
                }else {
                    uniform->set(*value);
                }
            }, uniformVar);
        }, var);
    }

    void UniformSource::initProvider(ShaderProgram* program) {
        providers.emplace_back(program->ID(), [program](const std::string& name) {
            return program->Registry().query(name);
        });
    }

    UniformSource::UniformSource(std::string prefix, const std::vector<ShaderProgram*>& programs) : glslPrefix(std::move(prefix)) {
        if (programs.empty())
            throw std::invalid_argument("Invalid argument; expected at least one shader program, but none were provided.");
        for (auto* program : programs) {
            initProvider(program);
        }
    }

    void UniformSource::relink(const std::vector<ShaderProgram*>& programs) {
        if (programs.empty())
            throw std::invalid_argument("Invalid argument; expected at least one shader program to"
                                        " perform hot reload, but none were provided.");
        providers.clear();
        for (auto* program : programs) {
            initProvider(program);
        }

        dump.clear();
        initDump();
    }

    void UniformSource::linkUniform(const std::string& name, const Dump& value) {
        for (const auto& [id, provider] : providers) {
            if (auto uniform = provider(glslPrefix + name)) {
                dump.emplace_back(uniform, name, value, id);
            }
        }
    }

    void UniformSource::draw(const ShaderProgram *provider) {
        if (dump.empty())
            return;

        for (const auto& entry : dump) {
            if (entry.provider == provider->ID()) {
                visit(entry.uniform, entry.value);
            }
        }
    }
}
