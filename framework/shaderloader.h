#ifndef PROJECTLABORATORY_SHADERLOADER_H
#define PROJECTLABORATORY_SHADERLOADER_H

#include <stdexcept>
#include <fstream>
#include "glad/glad.h"

using namespace std;

namespace Framework {
    class ShaderLoader {
        ShaderLoader() { }
        inline static string shaderCode;

        static string convertGLSLToString(const char* shader) {
            string code;
            string prefix = "glsl/";
            prefix.append(shader);

            ifstream shaderFile(prefix);
            if (!shaderFile.is_open()) {
                throw runtime_error("Error: unable to open shader file or file not found\n");
            }

            string line;
            while (getline(shaderFile, line)) {
                code.append(line + "\n");
            }
            shaderFile.close();

            return code;
        }
    public:
        static GLuint createAndCompileShader(unsigned int type, const char* source) {
            auto shaderID = glCreateShader(type);
            shaderCode = convertGLSLToString(source);
            const char* code = shaderCode.c_str();

            glShaderSource(shaderID, 1, &code, nullptr);
            glCompileShader(shaderID);

            GLint success, infoLogLength;
            glGetShaderiv(shaderID, type, &success);
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

            if (!success) {
                string infoLog(infoLogLength, '\0');
                glGetShaderInfoLog(shaderID, infoLogLength, nullptr, infoLog.data());
                throw runtime_error("Error compiling shader: " + to_string(type) + "\n" + infoLog.c_str());
            }

            return shaderID;
        }
    };
}

#endif //PROJECTLABORATORY_SHADERLOADER_H