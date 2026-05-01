#ifndef PROJECTLABORATORY_HDRTEXTURE_H
#define PROJECTLABORATORY_HDRTEXTURE_H

#include "framework/framework.h"

using namespace Framework;

struct HDRTexture : Texture2D {
    HDRTexture(const std::string &filePath);
};

#endif //PROJECTLABORATORY_HDRTEXTURE_H