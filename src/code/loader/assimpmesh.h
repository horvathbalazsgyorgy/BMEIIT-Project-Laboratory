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

    bool initialized;
    int iVertex, iIndex;

    static constexpr size_t chunk = 2000;
public:
    AssimpMesh(ShaderProgram* program, std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material* boundMaterial)
        : Mesh(program, boundMaterial), vertexData(std::move(vertices)), indices(std::move(indices)),
          initialized(false), iVertex(0), iIndex(0)
    { /*AssimpMesh::createMesh();*/ }

    void createMesh() override {
        glGenVertexArrays(1, &inputLayout);
        glGenBuffers(1, &vertexBuffer);
        glGenBuffers(1, &indexBuffer);

        glBindVertexArray(inputLayout);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vertex), nullptr, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);

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
    }

    bool streamMesh() {
        if (!initialized) {
            createMesh();
            initialized = true;
            return false;
        }

        if (iVertex != vertexData.size()) {
            const int voPtr = vertexData.size() - (iVertex + chunk);
            const unsigned int vOffset = voPtr <= 0 ? voPtr + chunk : chunk;
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * iVertex, sizeof(Vertex) * vOffset, &vertexData[iVertex]);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            iVertex += vOffset;
            return false;
        }

        if (iIndex != indices.size()) {
            const int ioPtr = indices.size() - (iIndex  + chunk);
            const unsigned int iOffset = ioPtr <= 0 ? ioPtr + chunk : chunk;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * iIndex, sizeof(unsigned int) * iOffset, &indices[iIndex]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            iIndex  += iOffset;
            return false;
        }

        vertexData.clear();
        vertexData.shrink_to_fit();
        return true;
    }

    void draw() override {
        glBindVertexArray(inputLayout);
        glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, 0);
    }
};

#endif //PROJECTLABORATORY_ASSIMPMESH_H