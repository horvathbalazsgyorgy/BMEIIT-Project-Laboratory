#ifndef PROJECTLABORATORY_FLICKERINGLIGHT_H
#define PROJECTLABORATORY_FLICKERINGLIGHT_H

#include <random>
#include "framework/framework.h"

using namespace Framework;

class Random {
    static inline std::mt19937 generator {std::random_device{}()};
    static inline std::uniform_real_distribution<float> distribution {0.0f, 1.0f};
public:
    static float RNG() {
        return distribution(generator);
    }
};

class FlickeringLight : public Light {
    float start, target, timer, interval;
    glm::vec2 transition;
    glm::vec3 min, max;
public:
    FlickeringLight(
        const std::vector<ShaderProgram*>& programs,
        const glm::vec3& lightMin,
        const glm::vec3& lightMax,
        const glm::vec2& transition,
        const std::string& prefix = "light"
    ) : Light(programs, prefix), transition(transition), min(lightMin), max(lightMax)
    {
        timer  = 0.0f;
        start  = Random::RNG();
        target = Random::RNG();
        interval = transition.x + Random::RNG() * transition.y;
    }

    void update(const float dt) {
        timer += dt;
        if (timer >= interval) {
            timer    = 0.0f;
            start    = target;
            target   = Random::RNG();
            interval = transition.x + Random::RNG() * transition.y;
        }

        float t = timer / interval;
        float factor = std::lerp(start, target, t);
        emittance = glm::mix(min, max, factor);
    }
};

#endif //PROJECTLABORATORY_FLICKERINGLIGHT_H