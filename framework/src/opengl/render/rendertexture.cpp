#include "rendertexture.h"

#include "../../message/variants/applicationwarning.h"

namespace Framework {
    /**RenderTexture2D**/
    RenderTexture2D::RenderTexture2D(
        const int width,
        const int height,
        const int nMipLevels,
        const TextureProperties &properties) : RenderTexture(properties)
    {
        auto filtering = properties.filter == TRILINEAR ? GL_LINEAR : properties.filter;
        createHandle();
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexStorage2D(GL_TEXTURE_2D, nMipLevels, properties.internalFormat, width, height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, properties.filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, properties.wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, properties.wrap);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void RenderTexture2D::bindBufferTexture(const GLenum attachment, const int mipLevel) const {
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, mipLevel);
    }

    void RenderTexture2D::makeMipmap() {
        if (properties.filter != TRILINEAR) {
            ApplicationWarning::ComponentMismatch("RenderTexture2D", "mipmap", "no trilinear filtering");
            return;
        }

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TRILINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    /**RenderTextureCube**/
    RenderTextureCube::RenderTextureCube(
        const int width,
        const int height,
        const int nMipLevels,
        const TextureProperties& properties) : RenderTexture(properties)
    {
        auto filtering = properties.filter == TRILINEAR ? GL_LINEAR : properties.filter;

        createHandle();
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glTexStorage2D(GL_TEXTURE_CUBE_MAP, nMipLevels, properties.internalFormat, width, height);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, properties.filter);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filtering);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, properties.wrap);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, properties.wrap);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, properties.wrap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void RenderTextureCube::bindBufferTexture(GLenum attachment, int mipLevel) const {
        auto face = mipLevel % 6;
        auto mip  = mipLevel / 6;
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, texture, mip);
    }

    void RenderTextureCube::makeMipmap() {
        if (properties.filter != TRILINEAR) {
            ApplicationWarning::ComponentMismatch("RenderTextureCube", "mipmap", "no trilinear filtering");
            return;
        }

        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, TRILINEAR);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
}