#ifndef PROJECTLABORATORY_FRAMEBUFFER_H
#define PROJECTLABORATORY_FRAMEBUFFER_H

#include <memory>
#include <vector>
#include "../application.h"
#include "rendertexture.h"
#include "rendertarget.h"
#include "glad/glad.h"

namespace Framework {
    class DefaultFramebuffer : public RenderTarget {
    public:
        DefaultFramebuffer(const int width = WindowSize::width, const int height = WindowSize::height)
            : RenderTarget(width, height) {}

        void resize(int width, int height) override;
        void bindBuffer(int mipLevel) override;
    };

    class Framebuffer : public RenderTarget {
    protected:
        std::vector<GLuint> framebuffers, depthBuffers;
        std::vector<GLenum> attachments;
        std::vector<std::unique_ptr<RenderTexture>> targets;
        int  nMip, nTarget;
        bool complete;

        Framebuffer(int width, int height) : RenderTarget(width, height), nMip(0), nTarget(0), complete(false) {}
        void completeFramebuffer();
    public:
        Framebuffer(
            int width,
            int height,
            int nMipLevel
        );

        virtual void bindTarget(
            int targetCount,
            GLenum internalFormat,
            GLenum format,
            GLenum type,
            GLint  wrap,
            TextureFiltering filter
        );

        void resize(int width, int height) override;
        void bindBuffer(int mipLevel) override;
        RenderTexture* operator[](const int index) const {
            return targets[index].get();
        }
    };
}

#endif //PROJECTLABORATORY_FRAMEBUFFER_H