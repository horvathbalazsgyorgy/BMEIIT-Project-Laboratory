#ifndef PROJECTLABORATORY_ASSIMPMODEL_H
#define PROJECTLABORATORY_ASSIMPMODEL_H

#include <vector>
#include <string>
#include <concepts>
#include <memory>
#include <unordered_map>

#include "framework/framework.h"

using namespace Framework;

//NOTE: Prone to changes
enum TextureType {
    DIFFUSE,
    SPECULAR,
    NORMAL,
    AMBIENT,
    HEIGHT
};

class AssimpMesh;
class AssimpMaterial;
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

struct AssimpUniformName {
    std::unordered_map<TextureType, std::string> nameMapping;
    void init();
    static int toAssimpTexture(TextureType type);
    std::string& operator[](TextureType type);
};

class AssimpModel : public Model {
    std::vector<TextureType> textureTypes;
    std::unordered_map<std::string, std::unique_ptr<Texture2D>> loadedTextures;
    AssimpUniformName uniformNameMapping;
    std::string directory;

    void initAssimpModel(const std::string& path, const std::vector<TextureType>& types);
public:
    template<typename ...Args>
    requires(
        sizeof...(Args) >= 1 &&
        (std::same_as<Args, TextureType> && ...)
    )
    AssimpModel(ShaderProgram* program, const std::string& path, Args... args) : Model(program) {
        initAssimpModel(path, {args...});
    }

    void load(const std::string& filePath);
    void processNode(aiNode* node, const aiScene* scene);
    AssimpMesh* processMesh(aiMesh* mesh, const aiScene* scene);
    void processMaterial(AssimpMaterial* material, aiMaterial* assimpMat, int type, const std::string& nameMapping);

    void draw() override {
        if (meshes.size() == materials.size()) {
            for (size_t i = 0; i < meshes.size(); i++) {
                materials[i]->draw();
                meshes[i]->draw();
            }
        }
    }
};

#endif //PROJECTLABORATORY_ASSIMPMODEL_H