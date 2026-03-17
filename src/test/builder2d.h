#ifndef PROJECTLABORATORY_BUILDER2D_H
#define PROJECTLABORATORY_BUILDER2D_H

#include <vector>
#include "framework/framework.h"
#include "triangle.h"

using namespace Framework;

class Builder2D : public SceneBuilder {
    ShaderProgram* program;
    Material* material;
    Mesh *mesh;

    std::vector<Model*> models;

    void buildPrograms() override {
        GLuint vertexShader = ShaderLoader::createAndCompileShader(GL_VERTEX_SHADER, "idle-vs.glsl");
        GLuint fragmentShader = ShaderLoader::createAndCompileShader(GL_FRAGMENT_SHADER, "triangle-fs.glsl");

        program = new ShaderProgram(vertexShader, fragmentShader);
    }
    void buildMeshes() override {
        mesh = new TriangleMesh();
        mesh->createMesh();
    }
    void buildMaterials() override {
        material = new Material(program);
        (*material)["kd"]->set(1.0f, 0.8f, 0.1f);
    }
    void buildModels() override {
        models.push_back(new Model(program, mesh, material, glm::vec3()));
    }
public:
    Builder2D() : SceneBuilder() {}

    void reset() override {
        models.clear();
    }

    void draw(float dt, std::set<unsigned int> keysPressed) override {
        glClearColor(0.3f, 0.0f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < models.size(); i++) {
            models[i]->draw();
        }
    }
};

#endif //PROJECTLABORATORY_BUILDER2D_H