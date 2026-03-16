#include "shaderprogram.h"

#include "../uniform/uniform.h"
#include <stdexcept>

namespace Framework {
    Uniform* ShaderProgram::queryUniform(const std::string& name) {
        Uniform* uniform = registry.query(name);
        if (uniform) {
            if (uniform->acceptTextureUnit(textureUnit))
                textureUnit++;
            return uniform;
        }
        return nullptr;
    }

    ShaderProgram::ShaderProgram(GLuint vertexShader, GLuint fragmentShader) {
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        GLint success, infoLogLength;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);

        if (!success) {
            std::string infoLog(infoLogLength, '\0');
            glGetProgramInfoLog(shaderProgram, infoLogLength, nullptr, infoLog.data());
            throw std::runtime_error("Error in linking shaders:\n" + infoLog);
        }

        registry.gatherUniforms(shaderProgram);
        textureUnit = 0;
    }

    void ShaderProgram::useShaderProgram() {
        textureUnit = 0;
        glUseProgram(shaderProgram);
    }

    ShaderProgram::~ShaderProgram() {
        if (shaderProgram > 0) {
            glDeleteProgram(shaderProgram);
        }
    }
}
