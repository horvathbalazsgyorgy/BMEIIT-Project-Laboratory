#ifndef PROJECTLABORATORY_FRAMEBUFFERCUBE_H
#define PROJECTLABORATORY_FRAMEBUFFERCUBE_H

#include "framebuffer.h"
#include "rendertexture.h"
#include "glad/glad.h"

namespace Framework {
    class FramebufferCube : public Framebuffer {
    public:
        FramebufferCube(
            int width,
            int height,
            int nMipLevel = 1
        );

        void bindTarget(
            int targetCount,
            GLenum internalFormat,
            GLenum format,
            GLenum type,
            GLint  wrap,
            TextureFiltering filter
        ) override;

        void bindBuffer(int mipFace) override;
    };
}

#endif //PROJECTLABORATORY_FRAMEBUFFERCUBE_H