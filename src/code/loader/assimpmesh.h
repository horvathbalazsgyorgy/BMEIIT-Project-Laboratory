#ifndef PROJECTLABORATORY_ASSIMPMESH_H
#define PROJECTLABORATORY_ASSIMPMESH_H

#include <utility>
#include <vector>
#include "framework/framework.h"
#include "glad/glad.h"
#include "glm/glm.hpp"

using namespace Framework;

struct Vertex {
    glm::vec3  position;
    glm::vec4  color;
    glm::vec2  texCoord[4];
    glm::vec3  normal;
    glm::vec3  tangent;
    glm::vec3  bitangent;
    glm::ivec4 blendIndices;
    glm::vec4  blendWeights;
};

class AssimpMesh : public Mesh {
    GLuint vertexBuffer, indexBuffer, inputLayout;
    std::vector<Vertex> vertexData;
    std::vector<unsigned int> indices;
    Material* boundMaterial;
public:
    AssimpMesh(ShaderProgram* program, std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material* boundMaterial)
        : Mesh(program, boundMaterial), vertexData(std::move(vertices)), indices(std::move(indices))
    {
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
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

        for (int i = 0; i < 4; i++) {
            glEnableVertexAttribArray(i + 2);
            glVertexAttribPointer(i + 2, 2, GL_FLOAT, GL_FALSE,
                sizeof(Vertex), (void*)(offsetof(Vertex, texCoord) + i * sizeof(glm::vec2)));
        }

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

        glEnableVertexAttribArray(9);
        glVertexAttribIPointer(9, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, blendIndices));

        glEnableVertexAttribArray(10);
        glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, blendWeights));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        vertexData.clear();
        vertexData.shrink_to_fit();
    }

    void draw() override {
        glBindVertexArray(inputLayout);
        glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, 0);
    }
};

#endif //PROJECTLABORATORY_ASSIMPMESH_H