#ifndef PROJECTLABORATORY_LIGHT_H
#define PROJECTLABORATORY_LIGHT_H

#include <string>
#include <vector>
#include "../uniform/uniformsource.h"
#include "glm/glm.hpp"

namespace Framework {
    class ShaderProgram;
    class Uniform;

    //NOTE: Prone to changes
    class Light : public UniformSource {
        std::vector<ShaderProgram*> programs;
        glm::vec4 position;
        glm::vec3 density;

        Uniform* searchUniform(const std::string& name) const;
    public:
        Light(std::vector<ShaderProgram*>& programs,
            glm::vec4 position = glm::vec4(0.0f),
            glm::vec3 density = glm::vec3(0.0f),
            const std::string& prefix = "light")
            : UniformSource(prefix), programs(programs), position(position), density(density) { }

        void setPosition(const glm::vec4 pos) { position = pos; }
        void setDensity(const glm::vec3 dense) { density = dense; }

        Uniform* operator[](const std::string& name) const override;
        ~Light() override = default;
    };
}

#endif //PROJECTLABORATORY_LIGHT_H