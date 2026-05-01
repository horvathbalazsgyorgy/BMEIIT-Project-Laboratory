#include "hdrtexture.h"
#include "stb_image.h"

HDRTexture::HDRTexture(const std::string &filePath) {
    stbi_set_flip_vertically_on_load(true);

    int width, height, channels;
    float* data = stbi_loadf(filePath.c_str(), &width, &height, &channels, 0);
    if (!data) {
        stbi_image_free(data);
        throw std::runtime_error("Failed to load HDR texture file at the following path: " + filePath);
    }

    GLint internalFormat = channels == 4 ? GL_RGBA16F : GL_RGB16F;
    GLenum format = channels == 3 ? GL_RGB : (channels == 4 ? GL_RGBA : GL_RED);
    GLint filter = GL_LINEAR;

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}