#ifndef PROJECTLABORATORY_HDRCUBE_H
#define PROJECTLABORATORY_HDRCUBE_H

#include <vector>
#include "framework/framework.h"

using namespace Framework;

struct Face {
    float pitch, yaw;
    glm::vec3 up;
};

class HDRCube : public Model {
    int face;
    Camera* cubeCamera;
    std::vector<Face> faces;
public:
    HDRCube(ShaderProgram* program, Mesh* quad, Camera* cubeCamera) : Model(program, {quad}), cubeCamera(cubeCamera), face(0) {
        faces = {
            Face(  0.0f,   0.0f,glm::vec3(0.0f,-1.0f, 0.0f)),
            Face(  0.0f, 180.0f,glm::vec3(0.0f,-1.0f, 0.0f)),
            Face( 90.0f, -90.0f,glm::vec3(0.0f, 0.0f, 1.0f)),
            Face(-90.0f, -90.0f,glm::vec3(0.0f, 0.0f,-1.0f)),
            Face(  0.0f,  90.0f,glm::vec3(0.0f,-1.0f, 0.0f)),
            Face(  0.0f, -90.0f,glm::vec3(0.0f,-1.0f, 0.0f)),
        };
        this->cubeCamera->setAspectRatio(1.0f);
        this->cubeCamera->setFov(90.0f);
        this->cubeCamera->setPlanes(0.1f, 10.0f);

        cubeCamera->setRotation(faces[face].pitch, faces[face].yaw);
        cubeCamera->configureCoordinateSystem(faces[face].up);
        cubeCamera->configureTransformation();
    }

    void draw(const ShaderProgram *provider) override {
        auto i = (++face) % 6;
        cubeCamera->setRotation(faces[i].pitch, faces[i].yaw);
        cubeCamera->configureCoordinateSystem(faces[i].up);
        cubeCamera->configureTransformation();

        for (auto* mesh : meshes) {
            mesh->draw(provider);
        }
    }
};

#endif //PROJECTLABORATORY_HDRCUBE_H