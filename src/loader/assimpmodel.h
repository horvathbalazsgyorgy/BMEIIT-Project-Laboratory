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
    Material* makeMaterial(std::unordered_map<std::string, std::string>& paths);
public:
    template<typename ...Args>
    requires(
        sizeof...(Args) >= 1 &&
        (std::same_as<Args, TextureType> && ...)
    )
    AssimpModel(ShaderProgram* program,
        const std::string& path,
        const glm::vec3& position,
        const glm::vec3& scale,
        Args... args)
    : Model(program, position, scale) {
        initAssimpModel(path, {args...});
    }

    void load(const std::string& filePath);
    void processNode(aiNode* node, const aiScene* scene, const glm::mat4& modelMatrix);
    AssimpMesh* processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& modelMatrix);
    void processTextureType(
        aiMaterial* assimpMat,
        int type,
        const std::string& nameMapping,
        std::unordered_map<std::string, std::string>& paths
    );

    void draw() override {
        for (auto mesh : meshes) {
            mesh->draw();
        }
    }
};

#endif //PROJECTLABORATORY_ASSIMPMODEL_H