#ifndef PROJECTLABORATORY_BUILDER3D_H
#define PROJECTLABORATORY_BUILDER3D_H

#include <vector>
#include "../../framework/scene/scenebuilder.h"

namespace Framework {
    class ShaderProgram;
    class Mesh;
    class Texture;
    class Material;
    class Camera;
    class Model;
}

using namespace Framework;

class Builder3D : public SceneBuilder {
    ShaderProgram *envMappedProgram, *colorProgram;
    Mesh *quadMesh;
    Texture *envTexture;
    Material *envMappedMaterial;
    Camera *camera;

    std::vector<Model*> models;

    void buildPrograms() override;
    void buildMeshes() override;
    void buildMaterials() override;
    void buildModels() override;
public:
    Builder3D() : SceneBuilder() {}

    void reset() override {
        models.clear();
    }

    void draw(float dt, std::set<unsigned int> keysPressed) override;
};

#endif //PROJECTLABORATORY_BUILDER3D_H