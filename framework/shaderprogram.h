#ifndef PROJECTLABORATORY_SHADER_H
#define PROJECTLABORATORY_SHADER_H

#include <string>
#include "uniformregistry.h"
#include "glad/glad.h"

namespace Framework {
    class Uniform;

    class ShaderProgram {
        GLuint shaderProgram;
        UniformRegistry registry;
        int textureUnit;

        Uniform* queryUniform(const std::string& name);
        friend class Material;
        friend class Camera;
        friend class Model;
    public:
        ShaderProgram(GLuint vertexShader, GLuint fragmentShader);
        void useShaderProgram();
        ~ShaderProgram();
    };
}

#endif //PROJECTLABORATORY_SHADER_H