#ifndef PROJECTLABORATORY_LIGHT_H
#define PROJECTLABORATORY_LIGHT_H

#include "glm/glm.hpp"

using namespace glm;

//NOTE: Prone to changes
class Light {
    vec4 position;
    vec3 density;
public:
    Light(vec4 position, vec3 density) : position(position), density(density) { }

    void set();
};

#endif //PROJECTLABORATORY_LIGHT_H