#ifndef PROJECTLABORATORY_MODEL_H
#define PROJECTLABORATORY_MODEL_H

#include "material.h"
#include "mesh.h"

namespace Framework {
    class Model {
        Mesh* mesh;
        Material* material;
    public:
        Model(Mesh* mesh, Material* material) : mesh(mesh), material(material) { }

        void draw() {
            material->draw();
            mesh->draw();
        }
    };
}

#endif //PROJECTLABORATORY_MODEL_H