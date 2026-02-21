#ifndef PROJECTLABORATORY_SCENE_H
#define PROJECTLABORATORY_SCENE_H

namespace Framework {
    class Scene {
    public:
        Scene() = default;
        virtual void buildScene() = 0;
        virtual void drawScene() = 0;
        virtual void disposeScene() = 0;
        virtual ~Scene() = default;
    };
}

#endif //PROJECTLABORATORY_SCENE_H