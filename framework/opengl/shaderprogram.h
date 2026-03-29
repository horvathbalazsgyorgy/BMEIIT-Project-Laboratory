#ifndef PROJECTLABORATORY_SHADER_H
#define PROJECTLABORATORY_SHADER_H

#include <string>
#include "../uniform/uniformregistry.h"
#include "glad/glad.h"

namespace Framework {
    class Uniform;

    class ShaderProgram {
        friend class UniformSource;

        GLuint shaderProgram;
        static inline GLuint activeProgram = 0;
        UniformRegistry registry;
        std::vector<UniformSource*> sources;

        void notify() const;
        Uniform* queryUniform(const std::string& name) const;
    public:
        ShaderProgram(GLuint vertexShader, GLuint fragmentShader);
        void subscribe(UniformSource* source) { sources.push_back(source); }
        void unsubscribe(UniformSource* source) { std::erase(sources, source); }
        void useShaderProgram(UniformSource* source = nullptr) const;
        ~ShaderProgram();
    };
}

#endif //PROJECTLABORATORY_SHADER_H