#ifndef PROJECTLABORATORY_SHADER_H
#define PROJECTLABORATORY_SHADER_H

#include "../uniform/uniformregistry.h"
#include "glad/glad.h"

namespace Framework {
    class Uniform;
    class UniformSource;

    class ShaderProgram {
        GLuint shaderProgram;
        UniformRegistry registry;
        std::vector<UniformSource*> sources;
    public:
        ShaderProgram(GLuint vertexShader, GLuint fragmentShader);
        [[nodiscard]] GLuint ID() const { return shaderProgram; }
        [[nodiscard]] UniformRegistry& Registry() { return registry; }

        void notify() const;
        void subscribe(UniformSource* source) { sources.push_back(source); }
        void unsubscribe(UniformSource* source) { std::erase(sources, source); }
        void useShaderProgram() const;
        ~ShaderProgram();
    };
}

#endif //PROJECTLABORATORY_SHADER_H