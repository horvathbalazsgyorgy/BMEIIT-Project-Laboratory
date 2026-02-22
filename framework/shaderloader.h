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
                throw std::runtime_error("ERROR:\nUnable to open shader file or file not found\n");
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

            int success = 1;
            char infoLog[512];
            glGetShaderiv(shaderID, type, &success);

            if (!success) {
                glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
                throw std::runtime_error("ERROR: SHADER COMPILATION FAILED\n"s + infoLog);
            }

            return shaderID;
        }
    };
}

#endif //PROJECTLABORATORY_SHADERLOADER_H