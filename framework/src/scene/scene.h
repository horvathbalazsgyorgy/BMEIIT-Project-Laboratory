#ifndef PROJECTLABORATORY_SCENE_H
#define PROJECTLABORATORY_SCENE_H

#include <set>

namespace Framework {
    class SceneBuilder;

    class Scene {
        SceneBuilder* builder;
    public:
        Scene() = default;
        Scene(SceneBuilder* builder) : builder(builder) {}

        void setBuilder(SceneBuilder* build) {
            this->builder = build;
        }
        virtual void buildScene() const;
        virtual void drawScene(float dt, const std::set<unsigned int> &keysPressed) const;
        virtual void disposeScene() const;
        virtual ~Scene() = default;
    };
}

#endif //PROJECTLABORATORY_SCENE_H