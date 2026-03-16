#include "texture.h"

#include <stdexcept>
#include "stb_image.h"

namespace Framework {
    /**Texture2D**/
    Texture2D::Texture2D(const std::string &filePath) {
        stbi_set_flip_vertically_on_load(true);

        int width, height, channels;
        unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
        if (!data) {
            stbi_image_free(data);
            throw std::runtime_error("Failed to load texture file at the following path: " + filePath);
        }
        glBindTexture(GL_TEXTURE_2D, texture);

        GLenum format = channels == 3 ? GL_RGB : (channels == 4 ? GL_RGBA : GL_RED);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
    }

    void Texture2D::bindTexture(int samplerIndex) const {
        glActiveTexture(GL_TEXTURE0 + samplerIndex);
        glBindTexture(GL_TEXTURE_2D, texture);
    }


    /**TextureCube**/
    TextureCube::TextureCube(const std::string (&faces)[6]) {
        stbi_set_flip_vertically_on_load(false);

        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        int width, height, channels;
        for (int i = 0; i < 6; i++) {
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
            if (!data) {
                stbi_image_free(data);
                throw std::runtime_error("Failed to load texture file for cubemap at the following path: " + faces[i]);
            }
            GLenum format = channels == 3 ? GL_RGB : (channels == 4 ? GL_RGBA : GL_RED);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void TextureCube::bindTexture(int samplerIndex) const {
        glActiveTexture(GL_TEXTURE0 + samplerIndex);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    }
}
