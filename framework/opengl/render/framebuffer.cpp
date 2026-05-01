#include "framebuffer.h"

#include <cmath>
#include <stdexcept>
#include "rendertexture.h"

namespace Framework {
    /**DefaultFramebuffer**/
    void DefaultFramebuffer::bindBuffer(const int mipLevel) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
    }

    void DefaultFramebuffer::resize(const int width, const int height) {
        this->width = width;
        this->height = height;
    }

    /**Framebuffer**/
    void Framebuffer::completeFramebuffer() {
        for (const auto buf : framebuffers) {
            glBindFramebuffer(GL_FRAMEBUFFER, buf);
            glDrawBuffers(nTarget, attachments.data());
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                throw std::runtime_error("Framebuffer configuration error; expected framebuffer to be complete, but found otherwise.");
        }
        complete = true;
    }

    Framebuffer::Framebuffer(
        const int width,
        const int height,
        const int nMipLevel)
    : RenderTarget(width, height), nTarget(0), complete(false)
    {
        nMip = nMipLevel < 2 ? 1 : nMipLevel;
        framebuffers.resize(nMip);
        depthBuffers.resize(nMip);
        glGenFramebuffers(nMip, framebuffers.data());
        glGenRenderbuffers(nMip, depthBuffers.data());
    }

    void Framebuffer::bindTarget(
        const int targetCount,
        const GLenum internalFormat,
        const GLenum format,
        const GLenum type,
        const GLint  wrap,
        const TextureFiltering filter)
    {
        if (nMip <= 1 && filter == TRILINEAR)
            throw std::invalid_argument("Invalid argument; expected mipmapped targets due"
                                        " to TRILINEAR filtering, but found otherwise.");
        if (complete)
            complete = false;

        for (int i = 0; i < targetCount; i++) {
            attachments.push_back(GL_COLOR_ATTACHMENT0 + nTarget + i);
            targets.push_back(std::make_unique<RenderTexture2D>(
                width, height, TextureProperties(internalFormat, format, type, wrap, filter))
            );
        }

        for (int mip = 0; mip < nMip; mip++) {
            unsigned int mipWidth  = width  * std::pow(0.5, mip);
            unsigned int mipHeight = height * std::pow(0.5, mip);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[mip]);
            glBindRenderbuffer(GL_RENDERBUFFER, depthBuffers[mip]);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, (int)mipWidth, (int)mipHeight);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffers[mip]);

            for (int i = 0; i < targetCount; i++) {
                targets[nTarget + i]->bindBufferTexture(attachments[nTarget + i], mip);
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        nTarget += targetCount;
    }

    void Framebuffer::resize(const int width, const int height) {
        if ((this->width == width && this->height == height) || (width == 0 || height == 0))
            return;

        this->width = width;
        this->height = height;

        std::vector<TextureProperties> properties(nTarget);
        for (int i = 0; i < nTarget; i++) {
            properties[i] = targets[i]->properties;
        }

        nTarget = 0;
        targets.clear();
        attachments.clear();
        for (const auto [internalFormat, format, type, wrap, filter] : properties) {
            this->bindTarget(1, internalFormat, format, type, wrap, filter);
        }
    }

    void Framebuffer::bindBuffer(const int mipLevel) {
        if (!complete) {
            completeFramebuffer();
        }
        unsigned int mipWidth  = width  * std::pow(0.5, mipLevel);
        unsigned int mipHeight = height * std::pow(0.5, mipLevel);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[mipLevel]);
        glViewport(0, 0, (int)mipWidth, (int)mipHeight);
    }

    void Framebuffer::syncDepth(const int mipLevel) const {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffers[mipLevel]);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(
            0, 0, width, height,
            0, 0, width, height,
            GL_DEPTH_BUFFER_BIT, GL_NEAREST
        );
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
