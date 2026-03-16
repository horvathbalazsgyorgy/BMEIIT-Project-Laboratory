#include "application.h"

#include <set>
#include <stdexcept>
#include "../scene/scene.h"
#include "../utility/mouse.h"
#include "glm/vec2.hpp"

namespace {
    struct CallbackManager {
        static inline std::set<unsigned int> keysPressed;

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
            Framework::WindowSize::width = width;
            Framework::WindowSize::height = height;
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

        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                Framework::mouse.setMouse(true);
            }
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
                Framework::mouse.setMouse(false);
            }
        }

        static void cursor_pos_callback(GLFWwindow* window, double xPos, double yPos) {
            Framework::mouse.setCursor(glm::vec2((float)xPos, (float)yPos));
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

    void GLApplication::createWindow(int width, int height) {
        WindowSize::width = width;
        WindowSize::height = height;
        window = glfwCreateWindow(width, height, "", nullptr, nullptr);
        if (window == nullptr) {
            glfwTerminate();
            throw std::runtime_error("Error: failed to create GLFW window");
        }

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, CallbackManager::framebuffer_size_callback);
        glfwSetKeyCallback(window, CallbackManager::key_callback);
        glfwSetMouseButtonCallback(window, CallbackManager::mouse_button_callback);
        glfwSetCursorPosCallback(window, CallbackManager::cursor_pos_callback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::runtime_error("Error: failed to initialize GLAD");
        }
    }

    void GLApplication::render(Scene* scene) {
        float timeAtThisFrame = 0.0f;
        float timeAtLastFrame = 0.0f;
        while (!glfwWindowShouldClose(window)) {
            timeAtThisFrame = (float)glfwGetTime();
            float dt = timeAtThisFrame - timeAtLastFrame;
            timeAtLastFrame = timeAtThisFrame;

            scene->drawScene(dt, CallbackManager::keysPressed);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwTerminate();
    }
}
