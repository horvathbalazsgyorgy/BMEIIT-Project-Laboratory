#ifndef PROJECTLABORATORY_UNIFORMREGISTRY_H
#define PROJECTLABORATORY_UNIFORMREGISTRY_H

#include <string>
#include <memory>
#include <unordered_map>
#include "glad/glad.h"

namespace Framework {
    class Uniform;

    class UniformRegistry {
        std::unordered_map<std::string, std::unique_ptr<Uniform>> uniforms;
    public:
        void gatherUniforms(GLuint program);
        Uniform* query(const std::string& name) const;
    };
}

#endif //PROJECTLABORATORY_UNIFORMREGISTRY_H