#ifndef PROJECTLABORATORY_SCENEBUILDER_H
#define PROJECTLABORATORY_SCENEBUILDER_H

namespace Framework {
    class SceneBuilder {
        virtual void buildPrograms() = 0;
        virtual void buildMeshes() = 0;
        virtual void buildMaterials() = 0;
        virtual void buildModels() = 0;
    public:
        virtual void reset() = 0;
        virtual void draw() = 0;
        void build() {
            reset();
            buildPrograms();
            buildMeshes();
            buildMaterials();
            buildModels();
        }
        virtual ~SceneBuilder() = default;
    };
}

#endif //PROJECTLABORATORY_SCENEBUILDER_H