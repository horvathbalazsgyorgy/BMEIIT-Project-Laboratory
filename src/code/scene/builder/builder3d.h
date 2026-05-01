#ifndef PROJECTLABORATORY_BUILDER3D_H
#define PROJECTLABORATORY_BUILDER3D_H

#include <vector>
#include "framework/framework.h"

using namespace Framework;

class Builder3D : public SceneBuilder {
    std::vector<Model*> models;
    ShaderBatch *defaultBatch, *postProcBatch;

    Camera *camera;
    LightArray *pbrLights, *maxBlinnLights;
    Mesh *backgroundMesh, *postProcMesh;
    Material *backgroundMaterial, *postProcMaterial;
    Texture *envTexture, *hdrTexture;

    DefaultFramebuffer* defaultFramebuffer;
    FramebufferCube* framebufferCube, *convolutedFBOCube, *prefilterFBOCube;
    Framebuffer* integratedFBO, *postProcFramebuffer;

    ShaderProgram* hotreload;
    MiscSource& miscellaneous = MiscSource::use();
    float exposure, LoD;
    float timeSinceLastHotreload = 0.0f;

    void precompute();
    void buildPrograms() override;
    void buildMeshes() override;
    void buildMaterials() override;
    void buildModels() override;
    void buildFramebuffers();
    void buildCamera();
    void buildLights();
    void buildUniforms();
public:
    Builder3D() : SceneBuilder(), exposure(1.0f), LoD(5) { }

    void reset() override {
        models.clear();
    }

    void build() override {
        SceneBuilder::build();
        buildFramebuffers();
        buildCamera();
        buildLights();
        buildUniforms();
    }

    void draw(float dt, std::set<unsigned int> keysPressed) override;
};

#endif //PROJECTLABORATORY_BUILDER3D_H