#include "rendertexture.h"

namespace Framework {
    /**RenderTexture2D**/
    RenderTexture2D::RenderTexture2D(
        const int width,
        const int height,
        const TextureProperties &properties) : RenderTexture(properties)
    {
        auto mipmap  = properties.filter == TRILINEAR;
        auto filtering = properties.filter == TRILINEAR ? GL_LINEAR : properties.filter;

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D, 0,
            properties.internalFormat, width, height, 0,
            properties.format, properties.type, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, properties.filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, properties.wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, properties.wrap);
        if (mipmap)
            glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void RenderTexture2D::bindBufferTexture(const GLenum attachment, const int mipLevel) const {
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, mipLevel);
    }

    void RenderTexture2D::makeMipmap() {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TRILINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    /**RenderTextureCube**/
    RenderTextureCube::RenderTextureCube(
        const int width,
        const int height,
        const TextureProperties& properties) : RenderTexture(properties)
    {
        auto mipmap  = properties.filter == TRILINEAR;
        auto filtering = properties.filter == TRILINEAR ? GL_LINEAR : properties.filter;

        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        for (int i = 0; i < 6; i++) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                properties.internalFormat, width, height, 0,
                properties.format, properties.type, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, properties.filter);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filtering);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, properties.wrap);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, properties.wrap);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, properties.wrap);
        if (mipmap)
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void RenderTextureCube::bindBufferTexture(GLenum attachment, int mipLevel) const {
        auto face = mipLevel % 6;
        auto mip  = mipLevel / 6;
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, texture, mip);
    }

    void RenderTextureCube::makeMipmap() {
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, TRILINEAR);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
}