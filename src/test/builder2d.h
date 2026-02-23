#ifndef PROJECTLABORATORY_BUILDER2D_H
#define PROJECTLABORATORY_BUILDER2D_H

#include "framework/framework.h"
#include "triangle.h"

using namespace Framework;

class Builder2D : public SceneBuilder {
    ShaderProgram* program;
    Material* material;
    Mesh *mesh;

    std::vector<Model*> models;

    vec3 color;

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
        color = vec3(1.0f, 0.8f, 0.1f);
        material = new Material(program, color);

        program->useShaderProgram();
        material->set("kd", color);
    }
    void buildModels() override {
        models.push_back(new Model(mesh, material));
    }
public:
    Builder2D() : SceneBuilder() {}

    void reset() override {
        models.clear();
    }

    void draw(std::set<unsigned int> keysPressed) override {
        if (keysPressed.find('a') != keysPressed.end()) {
            if (color.z > 0.1f) color.z -= 0.01f;
        }

        if (keysPressed.find('b') != keysPressed.end()) {
            if (color.z < 1.0f) color.z += 0.01f;
        }

        material->set("kd", color);

        for (int i = 0; i < models.size(); i++) {
            models[i]->draw();
        }
    }
};

#endif //PROJECTLABORATORY_BUILDER2D_H