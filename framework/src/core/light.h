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
    protected:
        glm::vec4 position;
        glm::vec3 emittance;
        glm::vec3 ambient;

        void initDump() override {
            this->linkUniform("position", &position);
            this->linkUniform("emittance", &emittance);
            this->linkUniform("ambient", &ambient);
        }
    public:
        Light(const std::vector<ShaderProgram*>& programs,
            const std::string& prefix = "light",
            const glm::vec4 position  = glm::vec4(0.0f),
            const glm::vec3 emittance = glm::vec3(1.0f),
            const glm::vec3 ambient   = glm::vec3(0.0f))
        : UniformSource(prefix + '.', programs), position(position), emittance(emittance), ambient(ambient)
        {
            for (auto* program : programs) {
                program->subscribe(this);
            }
            Light::initDump();
        }

        glm::vec4& Position()  { return position;  }
        glm::vec3& Emittance() { return emittance; }
        glm::vec3& Ambient()   { return ambient;   }
        void relink(const std::vector<ShaderProgram *> &programs) override {
            UniformSource::relink(programs);
            for (auto* program : programs) {
                program->subscribe(this);
            }
        }

        ~Light() override = default;
    };
}

#endif //PROJECTLABORATORY_LIGHT_H