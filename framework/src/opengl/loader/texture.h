#ifndef PROJECTLABORATORY_TEXTURE_H
#define PROJECTLABORATORY_TEXTURE_H

#include <string>
#include "../../threading/job.h"
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

        int width = 0;
        int height = 0;
        int channels = 0;
        unsigned int iPixel = 0;
        unsigned char* pixels = nullptr;
        bool initialized = false;
    };

    class Texture {
    protected:
        bool owner = false;
        GLuint texture = 0;

        void createHandle();
        Texture() = default;
        Texture(const unsigned int ID) { texture = ID; }
    public:
        Texture(const Texture& texture) = delete;
        Texture& operator=(const Texture& texture) = delete;

        [[nodiscard]] GLuint ID() const { return texture; }
        virtual void bindTexture(int samplerIndex) const = 0;
        virtual ~Texture();
    };


    class Texture2D : virtual public Texture, public JobVisitor {
        TextureJobState state;
        void createTexture();
        void streamTexture();
        static constexpr size_t chunk = 32768;
    protected:
        Texture2D() = default;
    public:
        Texture2D(TextureEncoding encoding, const std::string &filePath);
        Texture2D(unsigned int ID, TextureEncoding encoding, const std::string &filePath);
        void bindTexture(int samplerIndex) const override;
        void run() override;
        bool complete() override;
        ~Texture2D() override;
    };


    class TextureCube : virtual public Texture {
    protected:
        TextureCube() = default;
    public:
        TextureCube(TextureEncoding encoding, const std::string (&faces)[6]);
        void bindTexture(int samplerIndex) const override;
    };
}

#endif //PROJECTLABORATORY_TEXTURE_H