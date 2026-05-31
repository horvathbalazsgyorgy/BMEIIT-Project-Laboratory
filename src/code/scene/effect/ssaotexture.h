#ifndef PROJECTLABORATORY_SSAOTEXTURE_H
#define PROJECTLABORATORY_SSAOTEXTURE_H

#include "framework/framework.h"

using namespace Framework;

class SSAOTexture : public Texture2D {
public:
    SSAOTexture(const int width, const int height, const std::vector<glm::vec3>& noise) {
        createHandle();
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0,
            GL_RGB16F, width, height,
            0, GL_RGB, GL_FLOAT, noise.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

#endif //PROJECTLABORATORY_SSAOTEXTURE_H