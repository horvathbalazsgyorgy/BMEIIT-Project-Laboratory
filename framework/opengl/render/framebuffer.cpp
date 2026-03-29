#include "framebuffer.h"

#include <stdexcept>
#include "rendertexture.h"

namespace Framework {
    void DefaultFramebuffer::bindBuffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
    }

    Framebuffer::Framebuffer(
        const int targetCount,
        const int width,
        const int height,
        const GLenum internalFormat,
        const GLenum format,
        const GLenum type)
    : RenderTarget(width, height)
    {
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        std::vector<GLenum> attachments;
        for (int i = 0; i < targetCount; i++) {
            attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
            targets.push_back(std::make_unique<RenderTexture2D>(width, height, internalFormat, format, type));
            targets.back()->bindBufferTexture(attachments.back());
        }
        glDrawBuffers(targetCount, attachments.data());

        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Framebuffer configuration error; expected framebuffer to be complete, but found otherwise.");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::bindBuffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, width, height);
    }
}