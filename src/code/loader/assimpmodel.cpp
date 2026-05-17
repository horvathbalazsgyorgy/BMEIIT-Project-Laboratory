#include "assimpmodel.h"

#include "assimpmesh.h"
#include "assimpmaterial.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "glm/glm.hpp"

glm::mat4 aiMatrix4x4ToGlm4x4(const aiMatrix4x4& aiMatrix) {
    return glm::mat4(
        aiMatrix.a1, aiMatrix.b1, aiMatrix.c1, aiMatrix.d1,
        aiMatrix.a2, aiMatrix.b2, aiMatrix.c2,  aiMatrix.d2,
        aiMatrix.a3, aiMatrix.b3, aiMatrix.c3, aiMatrix.d3,
        aiMatrix.a4, aiMatrix.b4, aiMatrix.c4, aiMatrix.d4
    );
}

Material *AssimpModel::makeMaterial(ShaderProgram* program, const std::unordered_map<std::string, std::pair<std::string, int>>& textures) {
    for (auto material : materials) {
        if (material->compare(textures)) {
            return material;
        }
    }

    auto material = new AssimpMaterial(program);
    for (const auto& [name, data] : textures) {
        auto path = data.first;
        int channel = data.second;
        material->addTexture(name, path, channel, loadedTextures[path]);
    }

    materials.push_back(material);
    return material;
}

Texture* AssimpModel::makeRiggingTexture() const {
    std::vector<glm::mat4> buffer(rigging.nBones);
    for (int iBone = 0; iBone < rigging.nBones; iBone++) {
        buffer[iBone] = rigging.bones[iBone].boneModelMatrix;
    }
    return new BufferTexture(GL_RGBA32F, buffer);
}

void AssimpModel::load(const std::string& filePath) {
    Assimp::Importer importer;
    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
    const aiScene* scene = importer.ReadFile(filePath.c_str(),
        aiProcess_Triangulate
        | aiProcess_CalcTangentSpace
        | aiProcess_GenSmoothNormals
        | aiProcess_JoinIdenticalVertices
        | aiProcess_ImproveCacheLocality
        | aiProcess_LimitBoneWeights
        | aiProcess_FindDegenerates
        | aiProcess_FindInvalidData
        | aiProcess_OptimizeMeshes
        | aiProcess_OptimizeGraph);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        ApplicationError::FileNotFound("model", filePath);
        return;
    }

    directory = filePath.substr(0, filePath.find_last_of('/'));

    AssimpBuildContext buildContext{};
    buildContext.scene = scene;
    buildContext.node = scene->mRootNode;
    buildContext.modelMatrix = glm::mat4(1.0f);
    processNode(buildContext);

    //rootInverseTransformation = glm::inverse(aiMatrix4x4ToGlm4x4(scene->mRootNode->mTransformation));
    if (rigging.nBones > 1) {
        buildContext.node = scene->mRootNode;
        buildContext.modelMatrix = glm::mat4(1.0f);
        processBoneHierarchy(buildContext);
    }
}

void AssimpModel::run() {
    state.iterator = 0;
    load(context.path);
}

bool AssimpModel::complete() {
    if (state.iterator >= state.jobs.size()) {
        meshes.reserve(state.jobs.size());
        for (auto* mesh : state.jobs) {
            meshes.push_back(mesh);
        }
        state.jobs.clear();
        riggingTexture = makeRiggingTexture();

        AssimpModel::initDump();
        for (auto* material : materials) {
            material->initDump();
        }

        const std::string numBones = rigging.nBones > 1 ? "\tNum of bones: " + std::to_string(rigging.nBones) + "\n\n" : "\n";
        const std::string message = "Model loaded:\n"
                                    "\tMeshes: "    + std::to_string(meshes.size())    + "\n"
                                    "\tMaterials: " + std::to_string(materials.size()) + "\n"
                                                    + numBones;
        ApplicationInfo::GeneralInformation(message);
        return true;
    }

    const auto& mesh = state.jobs[state.iterator];
    if (mesh->streamMesh()) {
        state.iterator++;
    }
    return false;
}

void AssimpModel::processBoneHierarchy(AssimpBuildContext buildContext) {
    auto node = buildContext.node;
    buildContext.modelMatrix = buildContext.modelMatrix * aiMatrix4x4ToGlm4x4(node->mTransformation);

    auto iBone = rigging.boneIndices.find(node->mName.C_Str());
    if (iBone != rigging.boneIndices.end()) {
        auto& bone = rigging.bones[iBone->second];
        bone.boneModelMatrix = buildContext.modelMatrix * bone.boneOffsetMatrix;
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        buildContext.node = node->mChildren[i];
        processBoneHierarchy(buildContext);
    }
}

void AssimpModel::processNode(AssimpBuildContext buildContext) {
    auto node = buildContext.node;
    buildContext.modelMatrix = buildContext.modelMatrix * aiMatrix4x4ToGlm4x4(node->mTransformation);
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        buildContext.mesh = buildContext.scene->mMeshes[node->mMeshes[i]];
        state.jobs.push_back(processMesh(buildContext));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        buildContext.node = node->mChildren[i];
        processNode(buildContext);
    }
}

AssimpMesh* AssimpModel::processMesh(AssimpBuildContext& buildContext) {
    auto mesh = buildContext.mesh;
    std::vector<Vertex> vertices(mesh->mNumVertices);
    std::vector<unsigned int> indices;
    aiMaterial* assimpMaterial = buildContext.scene->mMaterials[mesh->mMaterialIndex];

    glm::mat4 transformationMatrix = buildContext.modelMatrix;
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transformationMatrix)));

    if (mesh->HasBones()) {
        transformationMatrix = glm::mat4(1.0f);
        normalMatrix = glm::mat3(1.0f);

        std::vector<int> nWeightsPerVertex(vertices.size());
        for (int i = 0; i < mesh->mNumBones; i++) {
            auto bone = mesh->mBones[i];
            auto boneIndex = rigging.makeBoneIndex(bone->mName.C_Str(), aiMatrix4x4ToGlm4x4(bone->mOffsetMatrix));

            for (int j = 0; j < bone->mNumWeights; j++) {
                auto iVertex = bone->mWeights[j].mVertexId;
                auto weight = bone->mWeights[j].mWeight;

                if (nWeightsPerVertex[iVertex] < 4) {
                    vertices[iVertex].blendIndices[nWeightsPerVertex[iVertex]] = boneIndex;
                    vertices[iVertex].blendWeights[nWeightsPerVertex[iVertex]] = weight;
                    nWeightsPerVertex[iVertex]++;
                }
            }
        }
    }else {
        auto boneName = "bone_Identity0";
        auto boneIndex = rigging.makeBoneIndex(boneName, glm::mat4(1.0f));
        for (auto& vertex : vertices) {
            vertex.blendIndices = glm::ivec4(boneIndex);
            vertex.blendWeights = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
        }
        rigging.bones[boneIndex].boneModelMatrix = glm::mat4(1.0f);
    }

    //std::cout << mesh->mName.C_Str() << std::endl;
    //std::cout << "UV channel: " << mesh->GetNumUVChannels() << std::endl;

    auto baseColor = aiColor4D(1.0f);
    if (assimpMaterial->Get(AI_MATKEY_BASE_COLOR, baseColor) != aiReturn_SUCCESS) {
        assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor);
    }

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        glm::vec3 position;
        position.x = mesh->mVertices[i].x;
        position.y = mesh->mVertices[i].y;
        position.z = mesh->mVertices[i].z;
        vertices[i].position = glm::xyz(transformationMatrix * glm::vec4(position, 1.0f));

        if (mesh->HasNormals()) {
            glm::vec3 normal;
            normal.x = mesh->mNormals[i].x;
            normal.y = mesh->mNormals[i].y;
            normal.z = mesh->mNormals[i].z;
            vertices[i].normal = glm::normalize(normalMatrix * normal);
        }

        auto numUV = mesh->GetNumUVChannels();
        unsigned int maxUV = numUV > 4 ? 4 : numUV;
        for (int j = 0; j < maxUV; j++) {
            glm::vec2 texCoord;
            texCoord.x = mesh->mTextureCoords[j][i].x;
            texCoord.y = mesh->mTextureCoords[j][i].y;
            vertices[i].texCoord[j] = texCoord;
        }

        auto vertexColor = glm::vec4(baseColor.r, baseColor.g, baseColor.b, baseColor.a);
        if (mesh->mColors[0]) {
            vertexColor.r *= mesh->mColors[0][i].r;
            vertexColor.g *= mesh->mColors[0][i].g;
            vertexColor.b *= mesh->mColors[0][i].b;
            vertexColor.a *= mesh->mColors[0][i].a;
        }
        vertices[i].color = vertexColor;

        if (mesh->HasTangentsAndBitangents()) {
            glm::vec3 tangent;
            tangent.x = mesh->mTangents[i].x;
            tangent.y = mesh->mTangents[i].y;
            tangent.z = mesh->mTangents[i].z;
            vertices[i].tangent = glm::normalize(normalMatrix * tangent);

            glm::vec3 bitangent;
            bitangent.x = mesh->mBitangents[i].x;
            bitangent.y = mesh->mBitangents[i].y;
            bitangent.z = mesh->mBitangents[i].z;
            vertices[i].bitangent = glm::normalize(normalMatrix * bitangent);
        }else {
            vertices[i].tangent = glm::normalize(normalMatrix * glm::vec3(1.0f, 0.0f, 0.0f));
            vertices[i].bitangent = glm::normalize(normalMatrix * glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    std::unordered_map<std::string, std::pair<std::string, int>> currentTextures;
    for (auto type : textureTypes) {
        processTextureType(
            type,
            assimpMaterial,
            uniformNameMapping[type],
            currentTextures
        );
    }

    auto material = makeMaterial(context.program, currentTextures);
    return new AssimpMesh(context.program, vertices, indices, material);
}

void AssimpModel::processTextureType(const TextureType type, const aiMaterial* assimpMat, const std::string& nameMapping, std::unordered_map<std::string, std::pair<std::string, int>>& textures) {
    int textureIncrement = 1;
    auto textureCount = assimpMat->GetTextureCount((aiTextureType)type);

    if (textureCount <= 0) {
        std::string name = nameMapping + "1";
        std::string path = "DummyDatabase/" + name;

        auto it = loadedTextures.find(path);
        if (it == loadedTextures.end()) {
            loadedTextures[path] = dummyTextures[type];
        }

        textures[name].first  = path;
        textures[name].second = 0;
        return;
    }

    for (unsigned int i = 0; i < textureCount; i++) {
        aiString textureName;
        assimpMat->GetTexture((aiTextureType)type, i, &textureName);

        int UV = 0;
        assimpMat->Get(AI_MATKEY_UVWSRC(type, i), UV);
        if (UV >= 4) {
            ApplicationError::ComponentNotSupported("model", "models which use more than 4 UV channels");
            return;
        }

        std::string texturePath = directory + '/' + textureName.C_Str();
        std::string uniformName = nameMapping + std::to_string(textureIncrement);
        textures[uniformName].first  = texturePath;
        textures[uniformName].second = UV;

        //std::cout << "Path: " << texturePath << " | UV channel: " << UV << std::endl;

        auto it = loadedTextures.find(texturePath);
        if (it == loadedTextures.end()) {
            TextureEncoding encoding = LINEAR;
            if (type == ALBEDO || type == DIFFUSE)
                encoding = sRGB;
            if (type == ROUGHNESS || type == METALLIC || type == AMBIENT_OCCLUSION)
                encoding = GRAYSCALE;

            loadedTextures[texturePath] = new Texture2D(dummyTextures[type]->ID(), encoding, texturePath);
            ThreadPool::enqueueJob(loadedTextures[texturePath]);
        }
        textureIncrement++;
    }
}