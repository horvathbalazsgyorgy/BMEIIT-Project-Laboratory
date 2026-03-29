#include "builder3d.h"
#include "quadmesh.h"
#include "src/loader/assimpmodel.h"

#include "framework/opengl/shaderprogram.h"
#include "framework/opengl/loader/shaderloader.h"
#include "../../framework/uniform/uniform.h"
#include "framework/core/camera.h"

void Builder3D::buildPrograms() {
    auto vsEnvmapped = ShaderLoader::createAndCompileShader(GL_VERTEX_SHADER, "envmapped-vs.glsl");
    auto fsEnvmapped = ShaderLoader::createAndCompileShader(GL_FRAGMENT_SHADER, "envmapped-fs.glsl");

    auto vsTrafo = ShaderLoader::createAndCompileShader(GL_VERTEX_SHADER, "trafo-vs.glsl");
    auto fsDiffuse = ShaderLoader::createAndCompileShader(GL_FRAGMENT_SHADER, "diffuse/diffuse-fs.glsl");

    envMappedProgram = new ShaderProgram(vsEnvmapped, fsEnvmapped);
    colorProgram = new ShaderProgram(vsTrafo, fsDiffuse);
}

void Builder3D::buildMeshes() {
    quadMesh = new QuadMesh(envMappedMaterial);
}

void Builder3D::buildMaterials() {
    envTexture = new TextureCube({
    "resources/background/nowhere/posx.jpg",
    "resources/background/nowhere/negx.jpg",
    "resources/background/nowhere/posy.jpg",
    "resources/background/nowhere/negy.jpg",
    "resources/background/nowhere/posz.jpg",
    "resources/background/nowhere/negz.jpg"});

    envMappedMaterial = new Material(envMappedProgram);
    (*envMappedMaterial)("envTexture", envMappedProgram)->set(envTexture);
}

void Builder3D::buildModels() {
    camera = new Camera({colorProgram, envMappedProgram}, glm::vec3(0.0f, 0.0f, 10.0f));
    models.push_back(new AssimpModel(colorProgram,
        "resources/assimp/revolver/scene.gltf",
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        DIFFUSE));
}

void Builder3D::draw(float dt, std::set<unsigned int> keysPressed) {
    glEnable(GL_DEPTH_TEST);

    glClearDepth(1.0f);
    glClearColor(0.3f, 0.0f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    colorProgram->useShaderProgram();
    camera->move(dt, keysPressed);

    for (auto model : models) {
        model->update();
        model->draw();
    }
    quadMesh->draw();
}