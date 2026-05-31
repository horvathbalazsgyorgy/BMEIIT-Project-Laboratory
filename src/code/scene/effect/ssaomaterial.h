#ifndef PROJECTLABORATORY_SSAOMATERIAL_H
#define PROJECTLABORATORY_SSAOMATERIAL_H

#include <random>
#include "ssaotexture.h"
#include "framework/framework.h"

using namespace Framework;

class SSAOMaterial : public Material {
    static inline std::random_device rd;
    static inline auto generator = std::default_random_engine(rd());

    int kernelSize;
    int resolution;
    Framebuffer* gBuffer;
    BufferTexture* kernel;
    SSAOTexture* noise;

    float lerp(const float a, const float b, const float t) { return a + t * (b - a); }
    void initDump() override;
    void makeKernel();
    void makeNoise();
public:
    SSAOMaterial(ShaderProgram* program, Framebuffer* gBuffer, int resolution, int kernelSize);
    ~SSAOMaterial() override {
        delete kernel;
        delete noise;
    }
};

#endif //PROJECTLABORATORY_SSAOMATERIAL_H