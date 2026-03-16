#ifndef PROJECTLABORATORY_ASSIMPMESH_H
#define PROJECTLABORATORY_ASSIMPMESH_H

#include <utility>
#include <vector>
#include "framework/framework.h"
#include "glad/glad.h"
#include "glm/glm.hpp"

using namespace Framework;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    //TODO: Tangent, Bitangent if needed
    //NOTE: Bones not needed (for now)
};

class AssimpMesh : public Mesh {
    GLuint vertexBuffer, indexBuffer, inputLayout;
    std::vector<Vertex> vertexData;
    std::vector<unsigned int> indices;
public:
    AssimpMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) :
        Mesh(), vertexData(std::move(vertices)), indices(std::move(indices)) {
		AssimpMesh::createMesh();
    }

    void createMesh() override {
        glGenVertexArrays(1, &inputLayout);
        glGenBuffers(1, &vertexBuffer);
        glGenBuffers(1, &indexBuffer);

        glBindVertexArray(inputLayout);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vertex), &vertexData[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void draw() override {
        glBindVertexArray(inputLayout);
        glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, 0);
    }
};

#endif //PROJECTLABORATORY_ASSIMPMESH_H