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
        void initDump() override {
            dump.variables[glslPrefix + ".position"]  = &position;
            dump.variables[glslPrefix + ".emittance"] = &emittance;
            dump.variables[glslPrefix + ".ambient"]   = &ambient;
        }
    protected:
        glm::vec4 position;
        glm::vec3 emittance;
        glm::vec3 ambient;
    public:
        Light(const std::vector<ShaderProgram*>& programs,
            const std::string& prefix = "light",
            const glm::vec4 position  = glm::vec4(0.0f),
            const glm::vec3 emittance = glm::vec3(1.0f),
            const glm::vec3 ambient   = glm::vec3(0.0f))
            : UniformSource(prefix, programs), position(position), emittance(emittance), ambient(ambient) { }

        void setPosition(const glm::vec4 pos) { position = pos; }
        void setEmittance(const glm::vec3 emit) { emittance = emit; }
        void setAmbient(const glm::vec3 amb) { ambient = amb; }

        ~Light() override = default;
    };
}

#endif //PROJECTLABORATORY_LIGHT_H