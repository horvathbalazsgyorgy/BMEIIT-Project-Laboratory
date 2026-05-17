#ifndef PROJECTLABORATORY_APPLICATION_H
#define PROJECTLABORATORY_APPLICATION_H

#include <stop_token>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "../message/messagequeue.h"

namespace Framework {
    class Scene;

    struct WindowSize {
        static inline int width;
        static inline int height;
    };

    class GLApplication {
        static inline bool initialized = false;
        static inline GLFWwindow* window;
        static inline std::stop_source source;
        static inline std::stop_token flag = source.get_token();
        static inline MessageQueue& messages = MessageQueue::getMessageQueue();
        GLApplication() = default;
    public:
        static bool Initialized() { return initialized; }
        static void initializeGL(int majorVersion, int minorVersion);
        static void createWindow(int width, int height);
        static void render(Scene* scene);
        static void shutdown();
    };
}

#endif //PROJECTLABORATORY_APPLICATION_H