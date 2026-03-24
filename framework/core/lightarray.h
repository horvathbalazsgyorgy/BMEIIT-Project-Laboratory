#ifndef PROJECTLABORATORY_LIGHTARRAY_H
#define PROJECTLABORATORY_LIGHTARRAY_H

#include <vector>
#include "light.h"

namespace Framework {
    class ShaderProgram;

    class LightArray {
        std::vector<Light*> lights;
    public:
        LightArray(int size, std::vector<ShaderProgram*>& programs) {
            lights.reserve(size);
            for (int i = 0; i < size; ++i) {
                lights.push_back(new Light(programs));
            }
        }

        Light& operator[](int index) {
            return *lights[index];
        }
    };
}

#endif //PROJECTLABORATORY_LIGHTARRAY_H