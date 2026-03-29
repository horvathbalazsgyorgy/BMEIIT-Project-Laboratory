#ifndef PROJECTLABORATORY_LIGHTARRAY_H
#define PROJECTLABORATORY_LIGHTARRAY_H

#include <string>
#include <vector>
#include <memory>
#include "light.h"

namespace Framework {
    class ShaderProgram;

    class LightArray {
        std::vector<std::unique_ptr<Light>> lights;
    public:
        LightArray(const int size, const std::vector<ShaderProgram*>& programs, const std::string& prefix = "lights") {
            lights.reserve(size);
            for (int i = 0; i < size; ++i) {
                lights.push_back(std::make_unique<Light>(programs, prefix + '[' + std::to_string(i) + ']'));
            }
        }

        LightArray& operator+=(const std::string& name) {
            for (auto& light : lights) {
                *light += name;
            }
            return *this;
        }

        Light& operator[](const int index) const {
            return *lights[index];
        }
    };
}

#endif //PROJECTLABORATORY_LIGHTARRAY_H