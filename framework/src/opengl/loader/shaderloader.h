#ifndef PROJECTLABORATORY_SHADERLOADER_H
#define PROJECTLABORATORY_SHADERLOADER_H

#include <string>
#include "glad/glad.h"

namespace Framework {
    class ShaderLoader {
        ShaderLoader() = default;
        static std::string convertGLSLToString(const char* shader);
    public:
        static GLuint createAndCompileShader(unsigned int type, const char* source);
    };
}

#endif //PROJECTLABORATORY_SHADERLOADER_H