#include "application.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void exit_key_callback(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
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
            throw std::runtime_error("Failed to create GLFW window");
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::runtime_error("Failed to initialize GLAD");
        }
    }

    void GLApplication::render() {
        while (!glfwWindowShouldClose(window)) {
            exit_key_callback(window);

            glClearColor(0.3f, 0.5f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwTerminate();
    }
}
