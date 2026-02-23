#ifndef PROJECTLABORATORY_SHADER_H
#define PROJECTLABORATORY_SHADER_H

#include <stdexcept>
#include "glad/glad.h"
#include "glm/glm.hpp"

using namespace std;
using namespace glm;

namespace Framework {
    class ShaderProgram {
        GLuint shaderProgram;
    public:
        ShaderProgram(GLuint vertexShader, GLuint fragmentShader) {
            shaderProgram = glCreateProgram();
            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            glLinkProgram(shaderProgram);

            GLint success, infoLogLength;
            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
            glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);

            if (!success) {
                string infoLog(infoLogLength, '\0');
                glGetProgramInfoLog(shaderProgram, infoLogLength, nullptr, infoLog.data());
                throw runtime_error("Error in linking shaders: "s + infoLog.c_str());
            }
        }

        void useShaderProgram() const {
            glUseProgram(shaderProgram);
        }

        //TODO: universal setUniform function
        //TODO: or more setUniform functions
        void setUniform(const char* name, const vec3 kd) const {
            auto loc = glGetUniformLocation(shaderProgram, name);
            if (loc < 0) {
                throw runtime_error("Error in setting the following uniform: "s + name
                    + "\nin program: " + to_string(shaderProgram));
            }
            glUniform3fv(loc, 1, &kd.x);
        }

        ~ShaderProgram() {
            if (shaderProgram > 0) {
                glDeleteProgram(shaderProgram);
            }
        }
    };
}

#endif //PROJECTLABORATORY_SHADER_H