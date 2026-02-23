#ifndef PROJECTLABORATORY_SCENE_H
#define PROJECTLABORATORY_SCENE_H

#include <iostream>
#include <set>
#include "scenebuilder.h"

using namespace std;

namespace Framework {
    class Scene {
        SceneBuilder* builder;
    public:
        Scene() = default;
        Scene(SceneBuilder* builder) : builder(builder) {}

        void buildScene() const {
            if (!builder) {
                throw runtime_error("Error!\n Builder in scene is not set! Can't build.");
            }
            builder->build();
        }
        void drawScene(const set<unsigned int> &keysPressed) const {
            if (!builder) {
                throw runtime_error("Error!\n Builder in scene is not set! Can't draw.");
            }
            builder->draw(keysPressed);
        }
        void disposeScene() const {
            if (!builder) {
                cout << "Warning!\n Builder in scene is not set! Can't dispose." << endl;
                return;
            }
            builder->reset();
        }

        void setBuilder(SceneBuilder* build) {
            this->builder = build;
        }
    };
}

#endif //PROJECTLABORATORY_SCENE_H