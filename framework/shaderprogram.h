#ifndef PROJECTLABORATORY_SHADER_H
#define PROJECTLABORATORY_SHADER_H

#include <stdexcept>
#include <string>

#include "uniform.h"
#include "uniformregistry.h"
#include "glad/glad.h"

using namespace std;

namespace Framework {
    class ShaderProgram {
        GLuint shaderProgram;
        UniformRegistry registry;
        int textureUnit;

        Uniform* queryUniform(const string& name) {
            Uniform* uniform = registry.query(name);
            if (uniform->acceptTextureUnit(textureUnit)) {
                textureUnit++;
            }
            return uniform;
        }

        friend class Material;
        friend class Camera;
        friend class Model;
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
                throw runtime_error("Error in linking shaders: "s + infoLog);
            }

            registry.gatherUniforms(shaderProgram);
            textureUnit = 0;
        }

        void useShaderProgram() {
            textureUnit = 0;
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