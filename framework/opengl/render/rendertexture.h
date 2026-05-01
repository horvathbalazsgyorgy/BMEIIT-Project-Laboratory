#ifndef PROJECTLABORATORY_RENDERTEXTURE_H
#define PROJECTLABORATORY_RENDERTEXTURE_H

#include "../loader/texture.h"
#include "glad/glad.h"

namespace Framework {
    enum TextureFiltering {
        POINT = GL_NEAREST,
        BILINEAR = GL_LINEAR,
        TRILINEAR = GL_LINEAR_MIPMAP_LINEAR,
    };

    struct TextureProperties {
        GLenum internalFormat, format, type;
        GLint  wrap;
        TextureFiltering filter;
    };

    class RenderTexture : virtual public Texture {
    public:
        TextureProperties properties;

        RenderTexture(const TextureProperties &properties) : properties(properties) {}
        virtual void bindBufferTexture(GLenum attachment, int mipLevel) const = 0;
        virtual void makeMipmap() = 0;
    };

    class RenderTexture2D : public RenderTexture, public Texture2D {
    public:
        RenderTexture2D(int width, int height, const TextureProperties &properties);
        void bindBufferTexture(GLenum attachment, int mipLevel) const override;
        void makeMipmap() override;
    };

    class RenderTextureCube : public RenderTexture, public TextureCube {
    public:
        RenderTextureCube(int width, int height, const TextureProperties &properties);
        void bindBufferTexture(GLenum attachment, int mipLevel) const override;
        void makeMipmap() override;
    };
}

#endif //PROJECTLABORATORY_RENDERTEXTURE_H