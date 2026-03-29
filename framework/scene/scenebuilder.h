#ifndef PROJECTLABORATORY_SCENEBUILDER_H
#define PROJECTLABORATORY_SCENEBUILDER_H

#include <set>

namespace Framework {
    class SceneBuilder {
        virtual void buildPrograms() = 0;
        virtual void buildMeshes() = 0;
        virtual void buildMaterials() = 0;
        virtual void buildModels() = 0;
    public:
        virtual void reset() = 0;
        virtual void draw(float dt, std::set<unsigned int> keysPressed) = 0;
        void build() {
            reset();
            buildPrograms();
            buildMaterials();
            buildMeshes();
            buildModels();
        }
        virtual ~SceneBuilder() = default;
    };
}

#endif //PROJECTLABORATORY_SCENEBUILDER_H