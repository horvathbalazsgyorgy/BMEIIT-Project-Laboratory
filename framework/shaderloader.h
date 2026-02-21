#ifndef PROJECTLABORATORY_SHADERLOADER_H
#define PROJECTLABORATORY_SHADERLOADER_H

#include <stdexcept>
#include "glad/glad.h"

using namespace std;

namespace Framework {
    class ShaderLoader {
        ShaderLoader() { }
    public:
        static unsigned int createAndCompileShader(unsigned int type, const char* source) {
            auto shaderID = glCreateShader(type);
            glShaderSource(shaderID, 1, &source, nullptr);
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