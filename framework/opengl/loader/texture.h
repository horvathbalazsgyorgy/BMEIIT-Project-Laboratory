#ifndef PROJECTLABORATORY_TEXTURE_H
#define PROJECTLABORATORY_TEXTURE_H

#include <string>
#include "glad/glad.h"

namespace Framework {
    enum TextureEncoding {
        GRAYSCALE = GL_RED,
        LINEAR = GL_RGB8,
        sRGB = GL_SRGB8
    };

    class Texture {
    protected:
        GLuint texture;
    public:
        Texture() {
            glGenTextures(1, &texture);
        }
        virtual void bindTexture(int samplerIndex) const = 0;
        virtual ~Texture() = default;
    };


    class Texture2D : virtual public Texture {
    public:
        Texture2D() = default;
        Texture2D(TextureEncoding encoding, const std::string &filePath);
        void bindTexture(int samplerIndex) const override;
    };


    class TextureCube : virtual public Texture {
    public:
        TextureCube() = default;
        TextureCube(TextureEncoding encoding, const std::string (&faces)[6]);
        void bindTexture(int samplerIndex) const override;
    };
}

#endif //PROJECTLABORATORY_TEXTURE_H