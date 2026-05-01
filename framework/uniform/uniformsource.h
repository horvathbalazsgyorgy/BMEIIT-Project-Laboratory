#ifndef PROJECTLABORATORY_UNIFORMSOURCE_H
#define PROJECTLABORATORY_UNIFORMSOURCE_H

#include <string>
#include <vector>
#include <variant>
#include <utility>
#include <concepts>
#include <stdexcept>

#include "../opengl/shaderprogram.h"
#include "glm/glm.hpp"

namespace Framework {
    class Texture;
    class Uniform;

    using DumpVar = std::variant<
        Texture*,
        glm::vec2*, glm::ivec2*, glm::mat2*,
        glm::vec3*, glm::ivec3*, glm::mat3*,
        glm::vec4*, glm::ivec4*, glm::mat4*,
        int*,       bool*,
        double*,    float*
    >;
    using UniformProvider = std::function<Uniform*(const std::string&)>;
    using DumpProvider    = std::function<DumpVar()>;
    using Dump = std::variant<DumpVar, DumpProvider>;

    struct DumpEntry {
        Uniform* uniform;
        std::string name;
        Dump       value;
        GLuint  provider;
    };

    struct ProviderEntry {
        GLuint id;
        UniformProvider provider;
    };

    class UniformSource {
        virtual void initDump() = 0;
    protected:
        std::string glslPrefix;
        std::vector<DumpEntry> dump;
        std::vector<ProviderEntry> providers;

        void initProvider(ShaderProgram* program);
    public:
        UniformSource(std::string prefix) : glslPrefix(std::move(prefix)) {}
        UniformSource(std::string prefix, const std::vector<ShaderProgram*>& programs);
        void linkUniform(const std::string& name, const Dump& value);
        virtual void relink(const std::vector<ShaderProgram*>& programs);
        virtual void draw(const ShaderProgram* provider);
        virtual ~UniformSource() = default;
    };

    class MiscSource : public UniformSource {
        void initDump() override { }
        MiscSource() : UniformSource("") {}
    public:
        template<typename... Args>
        requires(
            sizeof...(Args) >= 1 &&
            (std::same_as<Args, ShaderProgram*> && ...)
        )
        void linkPrograms(Args... args) {
            for (std::vector<ShaderProgram*> programs = {args...}; auto program : programs) {
                initProvider(program);
                program->subscribe(this);
            }
        }

        static MiscSource& use() {
            static MiscSource miscellaneous;
            return miscellaneous;
        }
    };
}

#endif //PROJECTLABORATORY_UNIFORMSOURCE_H