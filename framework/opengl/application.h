#ifndef PROJECTLABORATORY_APPLICATION_H
#define PROJECTLABORATORY_APPLICATION_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Framework {
    class Scene;

    struct WindowSize {
        static inline int width;
        static inline int height;
    };

    class GLApplication {
        static inline GLFWwindow* window;
        GLApplication() { }
    public:
        static void initializeGL(int majorVersion, int minorVersion);
        static void createWindow(int width, int height);
        static void render(Scene* scene);
    };
}

#endif //PROJECTLABORATORY_APPLICATION_H