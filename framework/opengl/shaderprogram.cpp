#include "shaderprogram.h"

#include <stdexcept>
#include "../uniform/uniform.h"
#include "framework/uniform/uniformsource.h"

namespace Framework {
    void ShaderProgram::notify() const {
        for (auto* source : sources) {
            source->update(this);
        }
    }

    Uniform* ShaderProgram::queryUniform(const std::string& name) const {
        if (auto* uniform = registry.query(name)) {
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
    }

    void ShaderProgram::useShaderProgram() const {
        glUseProgram(shaderProgram);
        notify();
    }

    ShaderProgram::~ShaderProgram() {
        if (shaderProgram > 0) {
            glDeleteProgram(shaderProgram);
        }
    }
}
