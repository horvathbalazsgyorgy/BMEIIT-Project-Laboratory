#ifndef PROJECTLABORATORY_ASSIMPMATERIAL_H
#define PROJECTLABORATORY_ASSIMPMATERIAL_H

#include <unordered_map>
#include "framework/framework.h"

using namespace Framework;

class AssimpMaterial : public Material {
    std::unordered_map<std::string, std::string> paths;
    std::unordered_map<std::string, Texture*> textures;
public:
    AssimpMaterial(ShaderProgram* program) :
        Material(program) {}

    void addTexture(const std::string& name, const std::string& path, Texture* texture) {
        paths[name] = path;
        textures[name] = texture;
    }

    bool compare(std::unordered_map<std::string, std::string>& currentPaths) {
        if (paths.size() != currentPaths.size()) {
            return false;
        }

        for (const auto& [name, path] : currentPaths) {
            auto it = paths.find(name);
            if (it == paths.end()) {
                return false;
            }
            if (it->second != path) {
                return false;
            }
        }

        return true;
    }

    void draw() override {
        Material::draw();
        for (const auto& [key, value]: textures) {
            (*this)[key]->set(value);
        }
    }
};

#endif //PROJECTLABORATORY_ASSIMPMATERIAL_H