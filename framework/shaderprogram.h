#ifndef PROJECTLABORATORY_SHADER_H
#define PROJECTLABORATORY_SHADER_H

#include <stdexcept>
#include "glad/glad.h"

using namespace std;

namespace Framework {
    class ShaderProgram {
        unsigned int shaderProgram;
    public:
        ShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) {
            shaderProgram = glCreateProgram();
            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            glLinkProgram(shaderProgram);

            int success = 1;
            char infoLog[512];
            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

            if (!success) {
                glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
                throw std::runtime_error("ERROR: SHADER LINKING FAILED\n"s + infoLog);
            }

            useShaderProgram();
        }
        void useShaderProgram() const {
            glUseProgram(shaderProgram);
        }

        ~ShaderProgram() {
            if (shaderProgram > 0) {
                glDeleteProgram(shaderProgram);
            }
        }
    };
}

#endif //PROJECTLABORATORY_SHADER_H