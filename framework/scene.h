#ifndef PROJECTLABORATORY_SCENE_H
#define PROJECTLABORATORY_SCENE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Framework {
    //TODO: early impl, outsource to Mesh, Geom, etc. class
    unsigned inline int vertexBuffer, indexBuffer, inputLayout;
    //Model* model;
    //Universal scene
    class Scene {
    public:
        Scene();
        //TODO: Abstract
        virtual void buildScene();
        virtual void drawScene();
        virtual void disposeScene();
        virtual ~Scene();
    };
}

#endif //PROJECTLABORATORY_SCENE_H