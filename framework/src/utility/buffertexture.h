#ifndef PROJECTLABORATORY_BUFFERTEXTURE_H
#define PROJECTLABORATORY_BUFFERTEXTURE_H

#include <vector>
#include "../opengl/loader/texture.h"
#include "glad/glad.h"

namespace Framework {
    class BufferTexture : public Texture {
        GLuint textureBuffer;
    public:
        template<typename Buf>
        BufferTexture(
            GLenum internalFormat,
            std::vector<Buf> buffer
        ) {
            glGenBuffers(1, &textureBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
            glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(Buf), buffer.data(), GL_STATIC_DRAW);

            glBindTexture(GL_TEXTURE_BUFFER, texture);
            glTexBuffer(GL_TEXTURE_BUFFER, internalFormat, textureBuffer);
            glBindTexture(GL_TEXTURE_BUFFER, 0);
        }

        void bindTexture(int samplerIndex) const override {
            glActiveTexture(GL_TEXTURE0 + samplerIndex);
            glBindTexture(GL_TEXTURE_BUFFER, texture);
        }
    };
}

#endif //PROJECTLABORATORY_BUFFERTEXTURE_H