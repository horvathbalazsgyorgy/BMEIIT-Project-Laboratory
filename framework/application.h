#ifndef PROJECTLABORATORY_CONTEXT_H
#define PROJECTLABORATORY_CONTEXT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "scene.h"

namespace Framework {
    class GLApplication {
        static inline GLFWwindow* window;
        GLApplication() { }
    public:
        static void initializeGL(int majorVersion, int minorVersion);
        static void createWindow(int height, int width);
        static void render(Scene* scene);
    };
}

#endif //PROJECTLABORATORY_CONTEXT_H