#include "shaderprogram.h"

#include <stdexcept>
#include "../uniform/uniformsource.h"

namespace Framework {
    void ShaderProgram::check() const {
        GLint success, infoLogLength;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        if (!success) {
            std::string infoLog(infoLogLength, '\0');
            glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.data());
            throw std::runtime_error("Error in linking shaders:\n" + infoLog);
        }
    }

    ShaderProgram::ShaderProgram(GLuint compute) : compute(true) {
        program = glCreateProgram();
        glAttachShader(program, compute);
        glLinkProgram(program);
        this->check();
        registry.gatherUniforms(program);
    }

    ShaderProgram::ShaderProgram(GLuint vertex, GLuint fragment, GLuint geometry) : compute(false) {
        program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        if (geometry > 0) glAttachShader(program, geometry);
        glLinkProgram(program);

        this->check();
       registry.gatherUniforms(program);
    }

    void ShaderProgram::use() const {
        glUseProgram(program);
    }

    void ShaderProgram::execute() const {
        if (compute)
            return;

        this->use();
        for (auto* source : sources) {
            source->draw(this);
        }
    }

    void ShaderProgram::dispatch(const glm::uvec3& groups, const GLuint barriers) const {
        if (!compute)
            throw std::runtime_error("Error dispatching shader; program does not consist of a compute shader.");

        this->use();
        for (auto* source : sources) {
            source->draw(this);
        }

        glDispatchCompute(groups.x, groups.y, groups.z);
        if (barriers > 0)
            glMemoryBarrier(barriers);
    }

    ShaderProgram::~ShaderProgram() {
        if (program > 0) {
            glDeleteProgram(program);
        }
    }
}
