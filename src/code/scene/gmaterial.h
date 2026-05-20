#ifndef PROJECTLABORATORY_GMATERIAL_H
#define PROJECTLABORATORY_GMATERIAL_H

#include "framework/framework.h"

using namespace Framework;

class GMaterial : public Material {
    Framebuffer *gBuffer;
    void initDump() override {
        this->linkUniform("gPosition", [this]{ return (*gBuffer)[0]; });
        this->linkUniform("gNormal",   [this]{ return (*gBuffer)[1]; });
        this->linkUniform("gAlbedo",   [this]{ return (*gBuffer)[2]; });
        this->linkUniform("gPBR",      [this]{ return (*gBuffer)[3]; });
    }
public:
    GMaterial(ShaderProgram* gBufProgram, Framebuffer* gBuffer) : Material(gBufProgram)
    {
        if (!gBuffer) {
            ApplicationError::MissingComponent("GMaterial", "Framebuffer (configured as G-Buffer)");
            return;
        }
        this->gBuffer = gBuffer;
        GMaterial::initDump();
    }
};

#endif //PROJECTLABORATORY_GMATERIAL_H