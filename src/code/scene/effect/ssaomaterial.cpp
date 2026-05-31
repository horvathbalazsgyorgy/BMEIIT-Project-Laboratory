#include "ssaomaterial.h"

void SSAOMaterial::initDump() {
    this->linkUniform("gNormal",   [this]{ return (*gBuffer)[0]; });
    this->linkUniform("kernelSize", &kernelSize);
    this->linkUniform("resolution", &resolution);
    this->linkUniform("kernel", kernel);
    this->linkUniform("noise",  noise);
}

void SSAOMaterial::makeKernel() {
    std::uniform_real_distribution<float> distribution(0.0, 1.0);
    std::vector<glm::vec3> ssaoKernel;
    for (unsigned int i = 0; i < kernelSize; i++) {
        glm::vec3 sample(
            distribution(generator) * 2.0f - 1.0f,
            distribution(generator) * 2.0f - 1.0f,
            distribution(generator)
        );

        sample = glm::normalize(sample);
        sample *= distribution(generator);
        float scale = float(i) / float(kernelSize);
        scale = lerp(0.1f, 1.0f, scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
    kernel = new BufferTexture(GL_RGB32F, ssaoKernel);
}

void SSAOMaterial::makeNoise() {
    std::uniform_real_distribution<float> distribution(0.0, 1.0);
    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < (resolution * resolution); i++) {
        glm::vec3 sample(
            distribution(generator) * 2.0f - 1.0f,
            distribution(generator) * 2.0f - 1.0f,
            0.0f
        );
        ssaoNoise.push_back(sample);
    }
    noise = new SSAOTexture(resolution, resolution, ssaoNoise);
}

SSAOMaterial::SSAOMaterial(ShaderProgram* program, Framebuffer* gBuffer, const int resolution, const int kernelSize) : Material(program) {
    if (!gBuffer) {
        ApplicationError::MissingComponent("SSAOMaterial", "Framebuffer (configured as G-Buffer)");
        return;
    }
    this->gBuffer = gBuffer;
    this->kernelSize = kernelSize;
    this->resolution = resolution;
    this->kernel = nullptr;
    this->noise  = nullptr;
    makeKernel();
    makeNoise();
    SSAOMaterial::initDump();
}