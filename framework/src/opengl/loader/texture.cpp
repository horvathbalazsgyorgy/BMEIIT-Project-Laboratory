#include "texture.h"

#include <filesystem>
#include "../../message/variants/applicationerror.h"
#include "config.h"
#include "stb_image.h"

namespace Framework {
    /**Texture2D**/
    void Texture2D::createTexture() {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        state.format = state.channels == 3 ? GL_RGB : (state.channels == 4 ? GL_RGBA : GL_RED);
        GLint internalFormat = state.encoding;
        if (state.encoding == sRGB   && state.channels == 4) internalFormat = GL_SRGB8_ALPHA8;
        if (state.encoding == LINEAR && state.channels == 4) internalFormat = GL_RGBA8;

        unsigned int mipCounter = 1;
        auto mipWidth = state.width;
        auto mipHeight = state.height;
        while (mipWidth != 1 || mipHeight != 1) {
            mipWidth  = std::max(1, (mipWidth  / 2));
            mipHeight = std::max(1, (mipHeight / 2));
            mipCounter++;
        }

        glTexStorage2D(GL_TEXTURE_2D, mipCounter, internalFormat, state.width, state.height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture2D::streamTexture() {
        const long lines = chunk / state.width;
        const long coPtr = state.height - (state.iPixel + lines);
        const long cOffset = coPtr <= 0 ? coPtr + lines : lines;
        glBindTexture(GL_TEXTURE_2D, texture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, state.channels);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0,
            state.iPixel, state.width, cOffset,
            state.format,GL_UNSIGNED_BYTE,
            &state.pixels[state.iPixel * state.width * state.channels]);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        state.iPixel += cOffset;
    }

    Texture2D::Texture2D(TextureEncoding encoding, const std::string &filePath) {
        stbi_set_flip_vertically_on_load(true);

        std::filesystem::path fullPath = std::filesystem::path(RESOURCES_PATH) / filePath;
        unsigned char* data = stbi_load(fullPath.string().c_str(), &state.width, &state.height, &state.channels, 0);
        if (!data) {
            stbi_image_free(data);
            ApplicationError::FileNotFound("texture", filePath);
            return;
        }
        GLenum format = state.channels == 3 ? GL_RGB : (state.channels == 4 ? GL_RGBA : GL_RED);
        GLint internalFormat = encoding;
        if (encoding == sRGB   && state.channels == 4) internalFormat = GL_SRGB8_ALPHA8;
        if (encoding == LINEAR && state.channels == 4) internalFormat = GL_RGBA8;

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, state.width, state.height, 0, format, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
    }

    Texture2D::Texture2D(const unsigned int ID, TextureEncoding encoding, const std::string &filePath) {
        texture = ID;
        state.path = filePath;
        state.encoding = encoding;
        state.iPixel = 0;
        state.initialized = false;
    }

    void Texture2D::bindTexture(int samplerIndex) const {
        glActiveTexture(GL_TEXTURE0 + samplerIndex);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void Texture2D::run() {
        stbi_set_flip_vertically_on_load(true);

        std::filesystem::path fullPath = std::filesystem::path(RESOURCES_PATH) / state.path;
        state.pixels = stbi_load(fullPath.string().c_str(), &state.width, &state.height, &state.channels, 0);

        if (!state.pixels) {
            stbi_image_free(state.pixels);
            ApplicationError::FileNotFound("texture", state.path);
        }
    }

    bool Texture2D::complete() {
        if (!state.initialized) {
            createTexture();
            state.initialized = true;
            return false;
        }

        if (state.iPixel != state.height) {
            streamTexture();
            return false;
        }

        glBindTexture(GL_TEXTURE_2D, texture);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(state.pixels);
        return true;
    }

    /**TextureCube**/
    TextureCube::TextureCube(TextureEncoding encoding, const std::string (&faces)[6]) {
        stbi_set_flip_vertically_on_load(false);

        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        int width, height, channels;
        for (int i = 0; i < 6; i++) {
            std::filesystem::path fullPath = std::filesystem::path(RESOURCES_PATH) / faces[i];
            unsigned char* data = stbi_load(fullPath.string().c_str(), &width, &height, &channels, 0);
            if (!data) {
                stbi_image_free(data);
                ApplicationError::FileNotFound("cubemap texture", faces[i]);
                return;
            }

            GLenum format = channels == 3 ? GL_RGB : (channels == 4 ? GL_RGBA : GL_RED);
            GLint internalFormat = encoding;
            if (encoding == sRGB   && channels == 4) internalFormat = GL_SRGB8_ALPHA8;
            if (encoding == LINEAR && channels == 4) internalFormat = GL_RGBA8;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void TextureCube::bindTexture(int samplerIndex) const {
        glActiveTexture(GL_TEXTURE0 + samplerIndex);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    }
}
