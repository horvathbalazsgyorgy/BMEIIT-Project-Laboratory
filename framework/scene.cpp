#include "scene.h"

namespace Framework {
    Scene::Scene() = default;

    void Scene::buildScene() {
        const float vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };

        const float indices[] = {
            0, 1, 2
        };

        //vertexBuffer
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        //indexBuffer
        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        //vao
        glGenVertexArrays(1, &inputLayout);
        glBindVertexArray(inputLayout);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }

    void Scene::drawScene() {
        glBindVertexArray(inputLayout);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
    }

    void Scene::disposeScene() {
        glDeleteVertexArrays(1, &inputLayout);
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &indexBuffer);
    }

    Scene::~Scene() {

    }
}
