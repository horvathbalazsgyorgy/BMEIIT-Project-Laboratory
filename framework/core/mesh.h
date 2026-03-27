#ifndef PROJECTLABORATORY_MESH_H
#define PROJECTLABORATORY_MESH_H

#include <stdexcept>
#include "material.h"

namespace Framework {
    class Mesh {
        Material* boundMaterial;
        void init() { createMesh(); }
    public:
        Mesh(Material* boundMaterial) {
            if (!boundMaterial)
                throw std::invalid_argument("Invalid argument; expected material but found none.");
            this->boundMaterial = boundMaterial;
            init();
        }

        virtual void createMesh() = 0;
        virtual void draw() {
            boundMaterial->draw();
        }
        virtual ~Mesh() = default;
    };
}

#endif //PROJECTLABORATORY_MESH_H