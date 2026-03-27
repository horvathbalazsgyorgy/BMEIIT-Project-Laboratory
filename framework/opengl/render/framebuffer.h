#ifndef PROJECTLABORATORY_FRAMEBUFFER_H
#define PROJECTLABORATORY_FRAMEBUFFER_H

#include <memory>
#include <vector>
#include "../application.h"
#include "rendertarget.h"
#include "glad/glad.h"

namespace Framework {
    class RenderTexture2D;

    class DefaultFramebuffer : public RenderTarget {
    public:
        DefaultFramebuffer(const int width = WindowSize::width, const int height = WindowSize::height)
            : RenderTarget(width, height) {}

        void bindBuffer() override;
    };

    class Framebuffer : public RenderTarget {
        GLuint framebuffer, depthBuffer;
        std::vector<std::unique_ptr<RenderTexture2D>> targets;
    public:
        Framebuffer(
            int targetCount = 1,
            int width = 512,
            int height = 512,
            GLenum internalFormat = GL_RGBA,
            GLenum format = GL_RGBA,
            GLenum type = GL_UNSIGNED_BYTE
        );

        void bindBuffer() override;
        RenderTexture2D* operator[](const int index) const {
            return targets[index].get();
        }
    };
}

#endif //PROJECTLABORATORY_FRAMEBUFFER_H