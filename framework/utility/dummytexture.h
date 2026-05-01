#ifndef PROJECTLABORATORY_DUMMYTEXTURE_H
#define PROJECTLABORATORY_DUMMYTEXTURE_H

#include <vector>
#include "../opengl/loader/texture.h"
#include "glad/glad.h"

namespace Framework {
    class DummyTexture : public Texture2D {
    public:
        DummyTexture(const GLenum internalFormat, const std::vector<unsigned char> &color) {
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, color.data());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    };
}

#endif //PROJECTLABORATORY_DUMMYTEXTURE_H