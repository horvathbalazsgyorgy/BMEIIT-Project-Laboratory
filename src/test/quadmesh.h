#ifndef PROJECTLABORATORY_QUADMESH_H
#define PROJECTLABORATORY_QUADMESH_H

#include "glad/glad.h"
#include "../../framework/core/mesh.h"

using namespace Framework;

class QuadMesh : public Mesh {
    GLuint vertexBuffer, vertexNormalBuffer, vertexTexCoordBuffer, indexBuffer, inputLayout;
public:
    QuadMesh(Material* boundMaterial) : Mesh(boundMaterial) { QuadMesh::createMesh(); }

    void createMesh() override {
        const float vertices[] = {
            -1.0f, -1.0f, 0.5f,
            -1.0f,  1.0f, 0.5f,
             1.0f, -1.0f, 0.5f,
             1.0f,  1.0f, 0.5f
        };

        const float normals[] = {
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f
        };

        const float texels[] = {
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };

        const unsigned short indices[] = {
            0, 1, 2,
            1, 2, 3
        };


        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &vertexNormalBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexNormalBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

        glGenBuffers(1, &vertexTexCoordBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexTexCoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texels), texels, GL_STATIC_DRAW);

        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &inputLayout);
        glBindVertexArray(inputLayout);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, vertexNormalBuffer);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, vertexTexCoordBuffer);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void draw() override {
        Mesh::draw();
        glBindVertexArray(inputLayout);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    }
};

#endif //PROJECTLABORATORY_QUADMESH_H