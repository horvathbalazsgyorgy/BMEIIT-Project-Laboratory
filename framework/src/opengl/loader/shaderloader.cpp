#include "shaderloader.h"

#include <fstream>
#include <filesystem>
#include "config.h"
#include "../../message/variants/applicationerror.h"

namespace Framework {
    std::string ShaderLoader::convertGLSLToString(const char* shader) {
        std::string code;
        std::filesystem::path shaderPath = std::filesystem::path(SHADER_PATH) / shader;

        std::ifstream shaderFile(shaderPath);
        if (!shaderFile.is_open()) {
            ApplicationError::FileNotFound("shader", shaderPath.string());
            return "";
        }

        std::string line;
        while (std::getline(shaderFile, line)) {
            code.append(line + "\n");
        }
        shaderFile.close();

        return code;
    }

    GLuint ShaderLoader::createAndCompileShader(unsigned int type, const char* source) {
        if (type == GL_TESS_CONTROL_SHADER || type == GL_TESS_EVALUATION_SHADER) {
            ApplicationError::ComponentNotSupported("shader", "tesselation shader");
            return 0;
        }

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

            std::string shaderType;
            if      (type == GL_VERTEX_SHADER)   { shaderType = "vertex";   }
            else if (type == GL_FRAGMENT_SHADER) { shaderType = "fragment"; }
            else if (type == GL_GEOMETRY_SHADER) { shaderType = "geometry"; }
            else if (type == GL_COMPUTE_SHADER)  { shaderType = "compute";  }
            else shaderType = "unknown";
            ApplicationError::GeneralConfigurationFailure("compile", shaderType + " shader", std::string(source), infoLog);
            return 0;
        }

        return shaderID;
    }
}
