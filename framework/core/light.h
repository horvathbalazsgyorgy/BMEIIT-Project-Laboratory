#ifndef PROJECTLABORATORY_LIGHT_H
#define PROJECTLABORATORY_LIGHT_H

#include <string>
#include "../uniform/uniformsource.h"
#include "glm/glm.hpp"

//NOTE: Prone to changes
namespace Framework {
    class Light : public UniformSource {
        glm::vec4 position;
        glm::vec3 density;
    public:
        Light(glm::vec4 position, glm::vec3 density, const std::string& prefix = "light")
            : UniformSource(prefix), position(position), density(density) { }

        //TODO: Implementation (need program for that)
        Uniform* operator[](const std::string& name) const override;
        ~Light() override = default;
    };
}

#endif //PROJECTLABORATORY_LIGHT_H