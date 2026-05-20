#ifndef PROJECTLABORATORY_RENDERTARGET_H
#define PROJECTLABORATORY_RENDERTARGET_H

namespace Framework {
    class RenderTarget {
    protected:
        int width;
        int height;
    public:
        RenderTarget(const int width, const int height) : width(width), height(height) {}
        virtual void resize(int width, int height) = 0;
        virtual void bindBuffer(int mipLevel) = 0;
        virtual ~RenderTarget() = default;
    };
}

#endif //PROJECTLABORATORY_RENDERTARGET_H