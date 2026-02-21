#ifndef PROJECTLABORATORY_TESTMESH_H
#define PROJECTLABORATORY_TESTMESH_H

#include "framework/framework.h"
#include "framework/mesh.h"

using namespace Framework;

class testMesh : public Mesh {
    unsigned int vertexBuffer, indexBuffer;
    unsigned int inputLayout;
    public:
    testMesh();
    void createMesh() override {
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float), nullptr, GL_STATIC_DRAW);

        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short), nullptr, GL_STATIC_DRAW);

        glGenVertexArrays(1, &inputLayout);
        glBindVertexArray(inputLayout);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    void draw() override {
        glBindVertexArray(inputLayout);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    }
};

#endif //PROJECTLABORATORY_TESTMESH_H