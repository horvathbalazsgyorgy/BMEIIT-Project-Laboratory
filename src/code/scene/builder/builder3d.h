#ifndef PROJECTLABORATORY_BUILDER3D_H
#define PROJECTLABORATORY_BUILDER3D_H

#include <vector>
#include "framework/framework.h"
#include "../camera/cinematic.h"
#include "../light/flickeringlight.h"

using namespace Framework;

class Builder3D : public SceneBuilder {
    std::vector<Model*> models;
    std::vector<FlickeringLight*> lights;
    ShaderBatch *gBufferBatch, *defaultBatch, *backgroundBatch, *preProcBatch, *postProcBatch;

    CinematicCamera *camera;
    Mesh *backgroundMesh, *postProcMesh, *gBufferMesh, *ssaoMesh, *bloomMesh;
    Material *backgroundMaterial, *postProcMaterial, *gBufferMaterial, *ssaoMaterial, *bloomMaterial;
    Texture *hdrTexture;

    DefaultFramebuffer* defaultFramebuffer;
    FramebufferCube* framebufferCube, *convolutedFBOCube, *prefilterFBOCube;
    Framebuffer* integratedFBO, *postProcFBO, *gBufferFBO, *ssaoFBO, *bloomFBO, *modernBloomFBO;

    ShaderProgram* hotreload, *ssaoShader, *bloomShader;
    MiscSource& miscellaneous = MiscSource::use();
    float exposure, LoD;
    float timeSinceLastHotreload  = 0.0f;
    float timeSinceLastIrradianceChange = 0.0f;
    float timeSinceLastSSAOSwitch = 0.0f;
    float timeSinceLastPBRSwitch  = 0.0f;
    float timeSinceLastCinematic  = 0.0f;
    float timeSinceLastBloom      = 0.0f;
    int   spherical = true,
          ambient   = true,
          pbrOld    = false,
          bloom     = true;
    int bloomMip, bloomMipLevel;

    GLuint ssbo1, ssbo2;

    void setupSSBO();
    void precompute();
    void buildPrograms() override;
    void buildMeshes() override;
    void buildMaterials() override;
    void buildModels() override;
    void buildFramebuffers();
    void buildCamera();
    void buildLights();
    void buildUniforms();
    void handleInput(const std::set<unsigned int>& keysPressed);
public:
    Builder3D() : SceneBuilder(), exposure(1.0f), LoD(4), bloomMip(6), bloomMipLevel(0) { }

    void reset() override {
        models.clear();
    }

    void build() override {
        reset();
        buildFramebuffers();
        buildPrograms();
        buildMaterials();
        buildMeshes();
        buildCamera();
        buildLights();
        buildUniforms();
        buildModels();
    }

    void draw(float dt, const std::set<unsigned int>& keysPressed) override;
};

#endif //PROJECTLABORATORY_BUILDER3D_H