#include "scene.h"

#include "scenebuilder.h"
#include "../message/variants/applicationerror.h"

namespace Framework {
    void Scene::buildScene() const {
        if (!builder) {
            ApplicationError::MissingComponent("Scene", "SceneBuilder to build");
            return;
        }

        if (GLApplication::Initialized())
            builder->build();
    }

    void Scene::drawScene(float dt, const std::set<unsigned int> &keysPressed) const {
        if (!builder) {
            ApplicationError::MissingComponent("Scene", "SceneBuilder to draw");
            return;
        }

        if (GLApplication::Initialized())
            builder->draw(dt, keysPressed);
    }

    void Scene::disposeScene() const {
        if (!builder) {
            ApplicationError::MissingComponent("Scene", "SceneBuilder to dispose");
            return;
        }

        if (GLApplication::Initialized())
            builder->reset();
    }
}