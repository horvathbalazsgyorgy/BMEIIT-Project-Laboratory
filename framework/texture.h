#ifndef PROJECTLABORATORY_TEXTURE_H
#define PROJECTLABORATORY_TEXTURE_H

#include <iostream>
#include <string>
#include <stb_image.h>
#include "glad/glad.h"

using namespace std;

namespace Framework {
    class Texture {
    protected:
        unsigned int texture;
    public:
        Texture() {
            glGenTextures(1, &texture);
        }
        void bindTexture(int samplerIndex) const {
            glActiveTexture(GL_TEXTURE0 + samplerIndex);
            glBindTexture(GL_TEXTURE_2D, texture);
        }
        virtual ~Texture() = default;
    };


    class Texture2D : public Texture {
    public:
        Texture2D(const string &filePath) {
            int width, height, channels;
            unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
            if (!data) {
                stbi_image_free(data);
                throw runtime_error("Failed to load texture file at the following path: " + filePath);
            }
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
            stbi_image_free(data);
        }
    };


    class TextureCube : public Texture {
    public:
        TextureCube(const string (&faces)[6]) {
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
            int width, height, channels;
            for (int i = 0; i < 6; i++) {
                unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
                if (!data) {
                    stbi_image_free(data);
                    throw runtime_error("Failed to load texture file for cubemap at the following path: " + faces[i]);
                }
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }
    };
}

#endif //PROJECTLABORATORY_TEXTURE_H