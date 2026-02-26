#include "application.h"
#include <iostream>

using namespace std;

namespace {
    struct CallbackManager {
        static inline set<unsigned int> keysPressed;

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
            glViewport(0, 0, width, height);
        }

        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            if ((mods & GLFW_MOD_SHIFT) == 0) key += 'a' - 'A';
            if (action == GLFW_PRESS || action == GLFW_REPEAT) keysPressed.insert(key);
            if (action == GLFW_RELEASE) keysPressed.erase(key);
        }
    };
}

namespace Framework {
    void GLApplication::initializeGL(int majorVersion, int minorVersion) {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    void GLApplication::createWindow(int height, int width) {
        window = glfwCreateWindow(width, height, "", nullptr, nullptr);
        if (window == nullptr) {
            glfwTerminate();
            throw runtime_error("Error: failed to create GLFW window");
        }

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, CallbackManager::framebuffer_size_callback);
        glfwSetKeyCallback(window, CallbackManager::key_callback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw runtime_error("Error: failed to initialize GLAD");
        }
    }

    void GLApplication::render(Scene* scene) {
        while (!glfwWindowShouldClose(window)) {
            scene->drawScene(CallbackManager::keysPressed);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwTerminate();
    }
}
