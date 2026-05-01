#include "framebuffercube.h"

#include <cmath>
#include <stdexcept>
#include "rendertexture.h"

namespace Framework {
    FramebufferCube::FramebufferCube(
        const int width,
        const int height,
        const int nMipLevel) : Framebuffer(width, height)
    {
        nMip = nMipLevel < 2 ? 1 : nMipLevel;
        framebuffers.resize(6 * nMip);
        depthBuffers.resize(nMip);
        glGenFramebuffers(6 * nMip, framebuffers.data());
        glGenRenderbuffers(nMip, depthBuffers.data());
    }

    void FramebufferCube::bindTarget(
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
            targets.push_back(std::make_unique<RenderTextureCube>(
                width, height, TextureProperties(internalFormat, format, type, wrap, filter))
            );
        }

        for (int mip = 0; mip < nMip; mip++) {
            unsigned int mipWidth  = width  * std::pow(0.5, mip);
            unsigned int mipHeight = height * std::pow(0.5, mip);
            glBindRenderbuffer(GL_RENDERBUFFER, depthBuffers[mip]);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, (int)mipWidth, (int)mipHeight);

            for (int i = 0; i < 6; i++) {
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[mip * 6 + i]);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffers[mip]);
                for (int j = 0; j < targetCount; j++) {
                    targets[nTarget + j]->bindBufferTexture(attachments[nTarget + j], (mip * 6 + i));
                }
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        nTarget += targetCount;
    }

    void FramebufferCube::bindBuffer(const int mipFace) {
        if (!complete) {
            completeFramebuffer();
        }
        auto mip = mipFace / 6;
        unsigned int mipWidth  = width  * std::pow(0.5, mip);
        unsigned int mipHeight = height * std::pow(0.5, mip);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[mipFace]);
        glViewport(0, 0, (int)mipWidth, (int)mipHeight);
    }
}
