#ifndef PROJECTLABORATORY_UNIFORMSOURCE_H
#define PROJECTLABORATORY_UNIFORMSOURCE_H

#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include "glm/glm.hpp"

namespace Framework {
    class ShaderProgram;
    class Texture;
    class Uniform;
    class UniformRegistry;

    struct Dump {
    private:
        using DumpVar = std::variant<
            Texture**,
            glm::vec2*, glm::mat2*,
            glm::vec3*, glm::mat3*,
            glm::vec4*, glm::mat4*,
            int*,       bool*,
            double*,    float*
        >;
    public:
        std::unordered_map<std::string, DumpVar> variables;
        std::vector<std::string> uniforms;
    };

    //TODO: Ability to modify variable dump
    class UniformSource {
        virtual void initDump() = 0;
        void initSource(const std::vector<ShaderProgram*>& programs);
    protected:
        Dump dump;
        std::string glslPrefix;
    public:
        UniformSource(const std::string &prefix, const std::vector<ShaderProgram*>& programs);
        virtual void update(const ShaderProgram* program);
        UniformSource& operator+=(const std::string& name);
        Uniform* operator()(const std::string& name, const ShaderProgram* program) const;
        virtual ~UniformSource() = default;
    };
}

#endif //PROJECTLABORATORY_UNIFORMSOURCE_H