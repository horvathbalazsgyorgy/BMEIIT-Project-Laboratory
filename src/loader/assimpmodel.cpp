#include "assimpmodel.h"

#include "assimpmesh.h"
#include "assimpmaterial.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/material.h"
#include "glm/glm.hpp"

glm::mat4 aiMatrix4x4ToGlm4x4(const aiMatrix4x4& aiMatrix) {
    return glm::mat4(
        aiMatrix.a1, aiMatrix.b1, aiMatrix.c1, aiMatrix.d1,
        aiMatrix.a2, aiMatrix.b2, aiMatrix.c2,  aiMatrix.d2,
        aiMatrix.a3, aiMatrix.b3, aiMatrix.c3, aiMatrix.d3,
        aiMatrix.a4, aiMatrix.b4, aiMatrix.c4, aiMatrix.d4
    );
}

void AssimpUniformName::init() {
    //NOTE: Prone to changes
    nameMapping[DIFFUSE] = "diffuseMap";
    nameMapping[SPECULAR] = "specularMap";
    nameMapping[NORMAL] = "normalMap";
    nameMapping[AMBIENT] = "ambientMap";
    nameMapping[HEIGHT] = "heightMap";
}

int AssimpUniformName::toAssimpTexture(TextureType type) {
   switch (type) {
       case DIFFUSE:
           return aiTextureType_DIFFUSE;
       case SPECULAR:
           return aiTextureType_SPECULAR;
       case AMBIENT:
           return aiTextureType_AMBIENT;
       case HEIGHT:
           return aiTextureType_HEIGHT;
       case NORMAL:
           return aiTextureType_NORMALS;
       default:
           return -1;
   }
}

std::string& AssimpUniformName::operator[](TextureType type) {
    return nameMapping[type];
}

void AssimpModel::initAssimpModel(const std::string& path, const std::vector<TextureType>& types) {
    textureTypes = std::vector(types);
    uniformNameMapping.init();
    load(path);
}

Material *AssimpModel::makeMaterial(std::unordered_map<std::string, std::string> &paths) {
    for (auto material : materials) {
        //It is certainly all AssimpMaterial stored in materials
        if (static_cast<AssimpMaterial*>(material)->compare(paths)) {
            return material;
        }
    }

    auto material = new AssimpMaterial(program);
    for (const auto& [name, path] : paths) {
        material->addTexture(name, path, loadedTextures[path].get());
    }
    materials.push_back(material);
    return material;
}

void AssimpModel::load(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filePath.c_str(),
        aiProcess_Triangulate
        | aiProcess_CalcTangentSpace
        | aiProcess_GenSmoothNormals
        | aiProcess_JoinIdenticalVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        //TODO: Unified Logger class in Framework to get rid of unnecessary exceptions like below
        throw std::runtime_error("Error loading Assimp model (" + filePath + "): " + importer.GetErrorString());

    directory = filePath.substr(0, filePath.find_last_of('/'));
    processNode(scene->mRootNode, scene, glm::mat4(1.0f));
}

void AssimpModel::processNode(aiNode* node, const aiScene* scene, const glm::mat4& modelMatrix) {
    glm::mat4 transformation = modelMatrix * aiMatrix4x4ToGlm4x4(node->mTransformation);
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, transformation));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, transformation);
    }
}

AssimpMesh* AssimpModel::processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& modelMatrix) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 position;
        position.x = mesh->mVertices[i].x;
        position.y = mesh->mVertices[i].y;
        position.z = mesh->mVertices[i].z;
        vertex.position = glm::xyz(modelMatrix * glm::vec4(position, 1.0f));

        if (mesh->HasNormals()) {
            glm::vec3 normal;
            normal.x = mesh->mNormals[i].x;
            normal.y = mesh->mNormals[i].y;
            normal.z = mesh->mNormals[i].z;
            vertex.normal = glm::normalize(normalMatrix * normal);
        }

        if (mesh->mTextureCoords[0]) {
            glm::vec2 texCoord;
            texCoord.x = mesh->mTextureCoords[0][i].x;
            texCoord.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoord = texCoord;
        }else {
            vertex.texCoord = glm::vec2(0.0f, 0.0f);
        }

        if (mesh->HasTangentsAndBitangents()) {
            glm::vec3 tangent;
            tangent.x = mesh->mTangents[i].x;
            tangent.y = mesh->mTangents[i].y;
            tangent.z = mesh->mTangents[i].z;
            vertex.tangent = glm::normalize(normalMatrix * tangent);

            glm::vec3 bitangent;
            bitangent.x = mesh->mBitangents[i].x;
            bitangent.y = mesh->mBitangents[i].y;
            bitangent.z = mesh->mBitangents[i].z;
            vertex.bitangent = glm::normalize(normalMatrix * bitangent);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    std::unordered_map<std::string, std::string> currentPaths;
    aiMaterial* assimpMaterial = scene->mMaterials[mesh->mMaterialIndex];
    for (auto type : textureTypes) {
        processTextureType(assimpMaterial, AssimpUniformName::toAssimpTexture(type), uniformNameMapping[type], currentPaths);
    }

    auto material = makeMaterial(currentPaths);
    return new AssimpMesh(vertices, indices, material);
}

void AssimpModel::processTextureType(aiMaterial* assimpMat, int type, const std::string& nameMapping, std::unordered_map<std::string, std::string>& paths) {
    int textureIncrement = 1;
    for (unsigned int i = 0; i < assimpMat->GetTextureCount((aiTextureType)type); i++) {
        aiString textureName;
        assimpMat->GetTexture((aiTextureType)type, i, &textureName);
        std::string texturePath = directory + '/' + textureName.C_Str();
        std::string uniformName = nameMapping + std::to_string(textureIncrement);
        paths[uniformName] = texturePath;

        auto it = loadedTextures.find(texturePath);
        if (it == loadedTextures.end()) {
            loadedTextures[texturePath] = make_unique<Texture2D>(texturePath);
        }
        textureIncrement++;
    }
}