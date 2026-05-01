#ifndef PROJECTLABORATORY_ASSIMPMATERIAL_H
#define PROJECTLABORATORY_ASSIMPMATERIAL_H

#include <ranges>
#include <utility>
#include <unordered_map>
#include "framework/framework.h"

using namespace Framework;

struct TextureData {
    std::string path;
    Texture* texture{};
    int channel = 0;

    TextureData() = default;
    TextureData(std::string path, Texture* texture, int channel)
        : path(std::move(path)), texture(texture), channel(channel) {}
};

class AssimpMaterial : public Material {
    std::unordered_map<std::string, TextureData> textures;
public:
    AssimpMaterial(ShaderProgram* program, const std::string& prefix = "material") : Material(program, prefix) { }

    void initDump() override {
        for (auto& [name, data] : textures) {
            this->linkUniform(name, data.texture);
            this->linkUniform(name + "_uv", &data.channel);
        }
    }

    void addTexture(const std::string& name, const std::string& path, int channel, Texture* texture) {
        textures[name].path = path;
        textures[name].texture = texture;
        textures[name].channel = channel;
    }

    bool compare(const std::unordered_map<std::string, std::pair<std::string, int>>& currentTextures)
    {
        if (textures.size() != currentTextures.size()) {
            return false;
        }

        for (const auto& [name, data] : currentTextures) {
            auto nameIt = textures.find(name);
            if (nameIt == textures.end()) return false;
            if (nameIt->second.path != data.first || nameIt->second.channel != data.second) return false;
        }

        return true;
    }
};

#endif //PROJECTLABORATORY_ASSIMPMATERIAL_H