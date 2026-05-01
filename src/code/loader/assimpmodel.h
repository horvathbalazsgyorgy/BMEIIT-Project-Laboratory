#ifndef PROJECTLABORATORY_ASSIMPMODEL_H
#define PROJECTLABORATORY_ASSIMPMODEL_H

#include <vector>
#include <string>
#include <memory>
#include <concepts>
#include <unordered_map>

#include "assimpmaterial.h"
#include "assimp/material.h"

#include "framework/framework.h"

using namespace Framework;

class AssimpMesh;
class AssimpMaterial;
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

/**
 * Representing arbitrary texture types.
 * @note Prone to changes in the future.
 */
enum TextureType {
    ALBEDO    = aiTextureType_BASE_COLOR,
    DIFFUSE   = aiTextureType_DIFFUSE,
    SPECULAR  = aiTextureType_SPECULAR,
    NORMAL    = aiTextureType_NORMALS,
    AMBIENT   = aiTextureType_AMBIENT,
    HEIGHT    = aiTextureType_HEIGHT,
    METALLIC  = aiTextureType_METALNESS,
    ROUGHNESS = aiTextureType_DIFFUSE_ROUGHNESS,
    EMISSIVE  = aiTextureType_EMISSIVE,
    TRANSMISSIVE = aiTextureType_TRANSMISSION,
    AMBIENT_OCCLUSION  = aiTextureType_AMBIENT_OCCLUSION,
    METALLIC_ROUGHNESS = aiTextureType_GLTF_METALLIC_ROUGHNESS
};

/**
 * Represents data structure for a single bone.
 */
struct BoneData {
    std::string name;
    glm::mat4 boneOffsetMatrix;
    glm::mat4 boneModelMatrix;

    BoneData() : boneOffsetMatrix(glm::mat4(1.0f)), boneModelMatrix(glm::mat4(1.0f)) {}
    BoneData(std::string  name, const glm::mat4& boneOffsetMatrix)
        : name(std::move(name)), boneOffsetMatrix(boneOffsetMatrix), boneModelMatrix(glm::mat4(1.0f)) {}
};

/**
 * Contains the rigged structure of the skeleton of the model.
 */
struct Rigging {
    int nBones = 0;
    std::vector<BoneData> bones;
    std::unordered_map<std::string, int> boneIndices;

    int makeBoneIndex(const std::string& name, const glm::mat4& boneOffsetMatrix) {
        auto boneIndex = boneIndices.find(name);
        if (boneIndex == boneIndices.end()) {
            const int bIndex = nBones;
            boneIndices.emplace(name, nBones++);
            bones.emplace_back(name, boneOffsetMatrix);
            return bIndex;
        }
        return boneIndex->second;
    }
};

/**
 * Immutable mapping between arbitrary texture types and their corresponding uniform name.
 * @note prone to changes, e.g. new mappings
 */
class AssimpUniformName {
    std::unordered_map<TextureType, std::string> nameMapping;
    AssimpUniformName() {
        nameMapping = {
            {ALBEDO,    "albedoMap"},     {DIFFUSE,   "diffuseMap"},
            {SPECULAR,  "specularMap"},   {NORMAL,    "normalMap"},
            {AMBIENT,   "ambientMap"},    {HEIGHT,    "heightMap"},
            {METALLIC,  "metallicMap"},   {ROUGHNESS, "roughnessMap"},
            {EMISSIVE,  "emissiveMap"},   {TRANSMISSIVE, "transmissionMap"},
            {AMBIENT_OCCLUSION, "aoMap"}, {METALLIC_ROUGHNESS, "mrMap"}
        };
    }
    ~AssimpUniformName() = default;
public:
    std::string& operator[](TextureType type) {
        return nameMapping[type];
    }
    static AssimpUniformName& getUniformNames() {
        static AssimpUniformName uniformNames;
        return uniformNames;
    }
};

/**
 * Immutable mapping between arbitrary texture types and their corresponding dummy textures.
 * @note prone to changes, e.g. new dummies
 */
class DummyDatabase {
    std::unordered_map<TextureType, std::unique_ptr<DummyTexture>> dummyTextures;
    DummyDatabase() {
        dummyTextures.emplace(ALBEDO,    std::make_unique<DummyTexture>(GRAYSCALE, std::vector<unsigned char>{0, 0, 0}));
        dummyTextures.emplace(SPECULAR,  std::make_unique<DummyTexture>(LINEAR,    std::vector<unsigned char>{255, 255, 255}));
        dummyTextures.emplace(NORMAL,    std::make_unique<DummyTexture>(LINEAR,    std::vector<unsigned char>{128, 128, 255}));
        dummyTextures.emplace(ROUGHNESS, std::make_unique<DummyTexture>(GRAYSCALE, std::vector<unsigned char>{128, 128, 128}));
        dummyTextures.emplace(METALLIC,  std::make_unique<DummyTexture>(GRAYSCALE, std::vector<unsigned char>{0, 0, 0}));
        dummyTextures.emplace(EMISSIVE,  std::make_unique<DummyTexture>(LINEAR,    std::vector<unsigned char>{0, 0, 0}));
        dummyTextures.emplace(AMBIENT_OCCLUSION,  std::make_unique<DummyTexture>(GRAYSCALE, std::vector<unsigned char>{255, 255, 255}));
        dummyTextures.emplace(METALLIC_ROUGHNESS, std::make_unique<DummyTexture>(LINEAR,    std::vector<unsigned char>{255, 128, 0}));
    }
    ~DummyDatabase() = default;
public:
    DummyTexture* operator[](TextureType type) {
        return type == DIFFUSE ? dummyTextures[ALBEDO].get() : dummyTextures[type].get();
    }
    static DummyDatabase& getDummyTextures() {
        static DummyDatabase dummyTextures;
        return dummyTextures;
    }
};

/**
 * Build context for easier passing of parameters between functions.
 */
struct AssimpContext {
    ShaderProgram* program;
    const aiScene* scene;
    const aiNode*  node;
    const aiMesh*  mesh;
    glm::mat4 modelMatrix;
};

class AssimpModel : public Model {
    std::string directory;
    std::vector<AssimpMaterial*> materials;
    std::vector<TextureType> textureTypes;
    std::unordered_map<std::string, Texture2D*> loadedTextures;

    DummyDatabase& dummyTextures = DummyDatabase::getDummyTextures();
    AssimpUniformName& uniformNameMapping = AssimpUniformName::getUniformNames();

    //Data for rigging
    Rigging rigging;
    Texture* riggingTexture;

    //Model counter for logging
    static inline int loaded = 0;

    //Functions for generating/querying materials and textures
    Material*  makeMaterial(ShaderProgram* program, const std::unordered_map<std::string, std::pair<std::string, int>>& textures);
    [[nodiscard]] Texture* makeRiggingTexture() const;

    //Processing functions
    void processBoneHierarchy(AssimpContext context);
    void processNode(AssimpContext context);
    AssimpMesh* processMesh(AssimpContext& context);
    void processTextureType(
        TextureType type,
        const aiMaterial* assimpMat,
        const std::string& nameMapping,
        std::unordered_map<std::string, std::pair<std::string, int>>& textures
    );
    void initDump() override {
        Model::initDump();
        this->linkUniform("boneBuffer", riggingTexture);
    }
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
        const glm::vec3& rotation,
        Args... args)
    : Model(program, position, scale, rotation) {
        textureTypes = {args...};
        load(path, AssimpContext(program));

        AssimpModel::initDump();
        for (auto* material : materials) {
            material->initDump();
        }
    }
    void load(const std::string& filePath, AssimpContext context);
};

#endif //PROJECTLABORATORY_ASSIMPMODEL_H