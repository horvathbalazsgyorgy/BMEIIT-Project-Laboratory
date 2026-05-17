#ifndef PROJECTLABORATORY_TEXTURE_H
#define PROJECTLABORATORY_TEXTURE_H

#include <string>
#include "framework/threading/job.h"
#include "glad/glad.h"

namespace Framework {
    enum TextureEncoding {
        GRAYSCALE = GL_R8,
        LINEAR = GL_RGB8,
        sRGB = GL_SRGB8
    };

    struct TextureJobState {
        std::string path;
        TextureEncoding encoding;
        GLenum format;
        int width, height, channels;
        unsigned int iPixel;
        unsigned char* pixels;
        bool initialized;
    };

    class Texture {
    protected:
        GLuint texture;
    public:
        Texture() { glGenTextures(1, &texture); }
        Texture(const unsigned int ID) { texture = ID; }
        [[nodiscard]] GLuint ID() const { return texture; }
        virtual void bindTexture(int samplerIndex) const = 0;
        virtual ~Texture() = default;
    };


    class Texture2D : virtual public Texture, public JobVisitor {
        TextureJobState state;
        void createTexture();
        void streamTexture();
        static constexpr size_t chunk = 32768;
    public:
        Texture2D() = default;
        Texture2D(TextureEncoding encoding, const std::string &filePath);
        Texture2D(unsigned int ID, TextureEncoding encoding, const std::string &filePath);
        void bindTexture(int samplerIndex) const override;
        void run() override;
        bool complete() override;
    };


    class TextureCube : virtual public Texture {
    public:
        TextureCube() = default;
        TextureCube(TextureEncoding encoding, const std::string (&faces)[6]);
        void bindTexture(int samplerIndex) const override;
    };
}

#endif //PROJECTLABORATORY_TEXTURE_H