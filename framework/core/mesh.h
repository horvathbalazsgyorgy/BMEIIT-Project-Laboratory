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

    //NOTE: For test purposes
    class EmptyMesh : public Mesh {
    public:
        void createMesh() override {
            /*nop*/
        }
        void draw() override {
            /*nop*/
        }
    };
}

#endif //PROJECTLABORATORY_MESH_H