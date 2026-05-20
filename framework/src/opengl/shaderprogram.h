#ifndef PROJECTLABORATORY_SHADER_H
#define PROJECTLABORATORY_SHADER_H

#include <vector>
#include "../uniform/uniformregistry.h"
#include "glad/glad.h"

namespace Framework {
    class Uniform;
    class UniformSource;

    class ShaderProgram {
        GLuint program;
        UniformRegistry registry;
        std::vector<UniformSource*> sources;
        bool compute;

        void check() const;
    public:
        ShaderProgram(GLuint compute);
        ShaderProgram(GLuint vertex, GLuint fragment, GLuint geometry = 0);
        [[nodiscard]] GLuint ID() const { return program; }
        [[nodiscard]] UniformRegistry& Registry() { return registry; }

        void use() const;
        void execute() const;
        void dispatch(const glm::uvec3& groups, GLuint barriers = 0) const;
        void subscribe(UniformSource* source) { sources.push_back(source); }
        void unsubscribe(UniformSource* source) { std::erase(sources, source); }
        ~ShaderProgram();
    };
}

#endif //PROJECTLABORATORY_SHADER_H