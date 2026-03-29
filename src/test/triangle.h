#ifndef PROJECTLABORATORY_TRIANGLE_H
#define PROJECTLABORATORY_TRIANGLE_H

#include "glad/glad.h"
#include "../../framework/core/mesh.h"

using namespace Framework;

class TriangleMesh : public Mesh {
    GLuint vertexBuffer, indexBuffer, inputLayout;
public:
    TriangleMesh(Material* boundMaterial) : Mesh(boundMaterial) { TriangleMesh::createMesh(); }

    void createMesh() override {
        const float vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };

        const unsigned int indices[] = {
            0, 1, 2
        };

        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &inputLayout);
        glBindVertexArray(inputLayout);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void draw() override {
        Mesh::draw();
        glBindVertexArray(inputLayout);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    }
};

#endif //PROJECTLABORATORY_TRIANGLE_H