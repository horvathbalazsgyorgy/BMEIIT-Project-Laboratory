#include "shaderloader.h"

#include <stdexcept>
#include <fstream>

namespace Framework {
    std::string ShaderLoader::convertGLSLToString(const char* shader) {
        std::string code;
        std::string prefix = "glsl/";
        prefix.append(shader);

        std::ifstream shaderFile(prefix);
        if (!shaderFile.is_open()) {
            throw std::runtime_error("Unable to open shader file \"" + prefix + "\" or file not found");
        }

        std::string line;
        while (std::getline(shaderFile, line)) {
            code.append(line + "\n");
        }
        shaderFile.close();

        return code;
    }

    GLuint ShaderLoader::createAndCompileShader(unsigned int type, const char* source) {
        auto shaderID = glCreateShader(type);
        std::string shaderCode = convertGLSLToString(source);
        const char* code = shaderCode.c_str();

        glShaderSource(shaderID, 1, &code, nullptr);
        glCompileShader(shaderID);

        GLint success, infoLogLength;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

        if (!success) {
            std::string infoLog(infoLogLength, '\0');
            glGetShaderInfoLog(shaderID, infoLogLength, nullptr, infoLog.data());

            std::string shaderType = type == GL_VERTEX_SHADER ? "vertex" : "fragment";
            throw std::runtime_error("Error compiling " + shaderType + " shader "
                                    "(" + std::string(source) + "):\n" + infoLog);
        }

        return shaderID;
    }
}
