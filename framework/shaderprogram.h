#ifndef PROJECTLABORATORY_SHADER_H
#define PROJECTLABORATORY_SHADER_H

#define STRINGIFY(x) #x

#include <stdexcept>
#include "glad/glad.h"
#include "glm/glm.hpp"

using namespace std;
using namespace glm;

namespace Framework {
    class ShaderProgram {
        GLuint shaderProgram;
        //UniformLoader* loader;
    public:
        ShaderProgram(GLuint vertexShader, GLuint fragmentShader) {
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

        //TODO: universal setUniform function
        void setUniform() {

        }

        ~ShaderProgram() {
            if (shaderProgram > 0) {
                glDeleteProgram(shaderProgram);
            }
        }
    };
}

#endif //PROJECTLABORATORY_SHADER_H