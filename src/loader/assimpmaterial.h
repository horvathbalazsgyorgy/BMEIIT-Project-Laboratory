#ifndef PROJECTLABORATORY_ASSIMPMATERIAL_H
#define PROJECTLABORATORY_ASSIMPMATERIAL_H

#include <unordered_map>
#include "framework/framework.h"

using namespace Framework;

class AssimpMaterial : public Material {
    std::unordered_map<std::string, Texture*> textures;
public:
    AssimpMaterial(ShaderProgram* program) :
        Material(program) {}

    void addTexture(const std::string& key, Texture* value) {
        textures[key] = value;
    }

    void draw() override {
        Material::draw();
        for (const auto& [key, value]: textures) {
            (*this)[key]->set(value);
        }
    }
};

#endif //PROJECTLABORATORY_ASSIMPMATERIAL_H