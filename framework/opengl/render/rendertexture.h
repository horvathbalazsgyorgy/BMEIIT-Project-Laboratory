#ifndef PROJECTLABORATORY_RENDERTEXTURE_H
#define PROJECTLABORATORY_RENDERTEXTURE_H

#include "../loader/texture.h"
#include "glad/glad.h"

namespace Framework {
    class RenderTexture2D : public Texture2D {
    public:
        RenderTexture2D(
            int width,
            int height,
            GLenum internalFormat,
            GLenum format,
            GLenum type
        );

        void bindBufferTexture(GLenum attachment) const;
    };
}

#endif //PROJECTLABORATORY_RENDERTEXTURE_H