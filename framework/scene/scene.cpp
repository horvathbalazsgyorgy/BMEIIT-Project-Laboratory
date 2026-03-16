#include "scene.h"

#include <iostream>
#include <stdexcept>
#include "scenebuilder.h"

namespace Framework {
    void Scene::buildScene() const {
        if (!builder) {
            throw std::runtime_error("Error!\n Builder in scene is not set! Can't build.");
        }
        builder->build();
    }

    void Scene::drawScene(float dt, const std::set<unsigned int> &keysPressed) const {
        if (!builder) {
            throw std::runtime_error("Error!\n Builder in scene is not set! Can't draw.");
        }
        builder->draw(dt, keysPressed);
    }

    void Scene::disposeScene() const {
        if (!builder) {
            std::cout << "Warning!\n Builder in scene is not set! Can't dispose." << std::endl;
            return;
        }
        builder->reset();
    }
}