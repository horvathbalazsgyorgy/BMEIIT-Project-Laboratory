#ifndef PROJECTLABORATORY_TEXTURE_H
#define PROJECTLABORATORY_TEXTURE_H

#include <string>
#include "glad/glad.h"

namespace Framework {
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


    class Texture2D : public Texture {
    public:
        Texture2D() = default;
        Texture2D(const std::string &filePath);
        void bindTexture(int samplerIndex) const override;
    };


    class TextureCube : public Texture {
    public:
        TextureCube() = default;
        TextureCube(const std::string (&faces)[6]);
        void bindTexture(int samplerIndex) const override;
    };
}

#endif //PROJECTLABORATORY_TEXTURE_H