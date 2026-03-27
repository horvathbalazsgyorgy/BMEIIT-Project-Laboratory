#ifndef PROJECTLABORATORY_LIGHTARRAY_H
#define PROJECTLABORATORY_LIGHTARRAY_H

#include <vector>
#include "light.h"

namespace Framework {
    class ShaderProgram;

    class LightArray {
        std::vector<Light*> lights;
    public:
        LightArray(const int size, const std::vector<ShaderProgram*>& programs, const std::string& prefix = "lights") {
            lights.reserve(size);
            for (int i = 0; i < size; ++i) {
                lights.push_back(new Light(programs, prefix));
            }
        }

        Light& operator[](const int index) const {
            return *lights[index];
        }
    };
}

#endif //PROJECTLABORATORY_LIGHTARRAY_H