#ifndef PROJECTLABORATORY_MESH_H
#define PROJECTLABORATORY_MESH_H

namespace Framework {
    class Mesh {
    public:
        Mesh() = default;
        virtual void createMesh() = 0;
        virtual void draw() = 0;
        virtual ~Mesh() = default;
    };
}

#endif //PROJECTLABORATORY_MESH_H