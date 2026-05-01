#include "builder3d.h"

#include <iostream>
#include "../quadmesh.h"
#include "../../loader/assimpmodel.h"
#include "../../hdr/hdrtexture.h"
#include "../../hdr/hdrcube.h"

void Builder3D::precompute() { //NOLINT
    //Initializing the required shaders
    auto precomputeBatch = std::make_unique<ShaderBatch>(std::vector<ShaderProperty>{
        {"equirectangular", "envmapped-vs.glsl", "equirectangular-fs.glsl"},
        {"convolution",     "envmapped-vs.glsl", "convolution-fs.glsl"},
        {"prefilter",       "envmapped-vs.glsl", "prefilter-fs.glsl"},
        {"brdf",            "quad-vs.glsl",      "convolution-brdf-fs.glsl"}
    });

    auto cubeCamera = std::make_unique<Camera>(std::vector{
        (*precomputeBatch)["equirectangular"],
        (*precomputeBatch)["convolution"],
        (*precomputeBatch)["prefilter"]},
        glm::vec3(0.0f)
    );

    //Converting the equirectangular map to a cubemap
    {
        framebufferCube->bindTarget(1, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, BILINEAR);
        auto material = std::make_unique<Material>((*precomputeBatch)["equirectangular"]);
        auto quad     = std::make_unique<QuadMesh>((*precomputeBatch)["equirectangular"], material.get());
        auto hdrCube  = std::make_unique<HDRCube>((*precomputeBatch)["equirectangular"], quad.get(), cubeCamera.get());
        material->linkUniform("equirectangularMap", hdrTexture);
        for (int i = 0; i < 6; i++) {
            framebufferCube->bindBuffer(i);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            precomputeBatch->executeOne("equirectangular");
        }
        (*framebufferCube)[0]->makeMipmap();
    }

    //Convoluting the previously converted cubemap
    {
        convolutedFBOCube->bindTarget(1, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, BILINEAR);
        auto material = std::make_unique<Material>((*precomputeBatch)["convolution"]);
        auto quad     = std::make_unique<QuadMesh>((*precomputeBatch)["convolution"], material.get());
        auto hdrCube  = std::make_unique<HDRCube>((*precomputeBatch)["convolution"], quad.get(), cubeCamera.get());
        material->linkUniform("envTexture", (*framebufferCube)[0]);
        for (int i = 0; i < 6; i++) {
            convolutedFBOCube->bindBuffer(i);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            precomputeBatch->executeOne("convolution");
        }
    }

    //Pre-filtering the previously converted cubemap
    {
        float roughness;
        prefilterFBOCube->bindTarget(1, GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, TRILINEAR);
        auto material = std::make_unique<Material>((*precomputeBatch)["prefilter"]);
        auto quad     = std::make_unique<QuadMesh>((*precomputeBatch)["prefilter"], material.get());
        auto hdrCube  = std::make_unique<HDRCube>((*precomputeBatch)["prefilter"], quad.get(), cubeCamera.get());
        material->linkUniform("envTexture", (*framebufferCube)[0]);
        material->linkUniform("roughness", &roughness);
        const unsigned int nMipLevel = 5;
        for (int mip = 0; mip < nMipLevel; mip++) {
            roughness = (float)mip / (float)(nMipLevel - 1);
            for (int i = 0; i < 6; i++) {
                prefilterFBOCube->bindBuffer((mip * 6) + i);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                precomputeBatch->executeOne("prefilter");
            }
        }
    }

    //Convoluting the BRDF and creating the LUT
    {
        integratedFBO->bindTarget(1, GL_RG16F, GL_RG, GL_FLOAT, GL_CLAMP_TO_EDGE, BILINEAR);
        auto material = std::make_unique<Material>((*precomputeBatch)["brdf"]);
        auto quad     = std::make_unique<QuadMesh>((*precomputeBatch)["brdf"], material.get());
        auto model    = std::make_unique<Model>((*precomputeBatch)["brdf"], std::vector<Mesh*>{quad.get()});
        integratedFBO->bindBuffer(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        precomputeBatch->executeOne("brdf");
    }

    //Binding back the default framebuffer
    defaultFramebuffer->bindBuffer(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Builder3D::buildPrograms() {
    defaultBatch = new ShaderBatch({
        {"background","envmapped-vs.glsl", "envmapped-fs.glsl"},    //EnvmappedProgram
        {"maxblinn",  "skinning-vs.glsl",  "maxblinn-fs.glsl"},     //MaxBlinnProgram
        {"pbr",       "skinning-vs.glsl",  "cook-torrance-fs.glsl"} //PBRProgram
    });

    postProcBatch = new ShaderBatch({
        {"postproc", "quad-vs.glsl", "postproc-fs.glsl"} //PostProcProgram
    });
}

void Builder3D::buildMeshes() {
    backgroundMesh = new QuadMesh((*defaultBatch)["background"], backgroundMaterial);
    postProcMesh = new QuadMesh((*postProcBatch)["postproc"], postProcMaterial);
}

void Builder3D::buildMaterials() {
    envTexture = new TextureCube(LINEAR, {
    "resources/background/ldr/nowhere/posx.jpg",
    "resources/background/ldr/nowhere/negx.jpg",
    "resources/background/ldr/nowhere/posy.jpg",
    "resources/background/ldr/nowhere/negy.jpg",
    "resources/background/ldr/nowhere/posz.jpg",
    "resources/background/ldr/nowhere/negz.jpg"});

    hdrTexture = new HDRTexture("resources/background/hdr/qwantani_dusk_2_4k.hdr");
    backgroundMaterial = new Material((*defaultBatch)["background"]);
    postProcMaterial = new Material((*postProcBatch)["postproc"]);
}

void Builder3D::buildModels() {
    models.push_back(new Model((*postProcBatch)["postproc"], {postProcMesh}));
    models.push_back(new Model((*defaultBatch)["background"], {backgroundMesh}));

    hotreload = (*defaultBatch)["maxblinn"];
    models.push_back(new AssimpModel(hotreload,
        "resources/assimp/showroom/forgotten_knight/scene.gltf",
        glm::vec3(0.0f, 0.0f, 0.0f),
          glm::vec3(2.0f, 2.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        ALBEDO, NORMAL, AMBIENT_OCCLUSION, METALLIC_ROUGHNESS));

    std::cout << std::endl;
}

void Builder3D::buildFramebuffers() {
    defaultFramebuffer  = new DefaultFramebuffer();
    framebufferCube     = new FramebufferCube(1024, 1024, 1);
    convolutedFBOCube   = new FramebufferCube(64, 64, 1);
    prefilterFBOCube    = new FramebufferCube(128, 128, (int)LoD);
    integratedFBO       = new Framebuffer(512, 512, 1);
    postProcFramebuffer = new Framebuffer(WindowSize::width, WindowSize::height, 1);
    postProcFramebuffer->bindTarget(1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, POINT);
}

void Builder3D::buildCamera() {
    camera = new Camera(
        {(*defaultBatch)["background"],
                    (*defaultBatch)["maxblinn"],
                    (*defaultBatch)["pbr"]},
        glm::vec3(0.0f, 0.0f, 10.0f)
    );
    camera->setSpeed(5.0f);
}

void Builder3D::buildLights() {
    maxBlinnLights = new LightArray(3, {(*defaultBatch)["maxblinn"]});
    (*maxBlinnLights)[0].setPosition(glm::vec4(5.0f, 3.0f, -10.0f, 1.0f));
    (*maxBlinnLights)[0].setEmittance(glm::vec3(30.0f, 30.0f, 30.0f));
    (*maxBlinnLights)[0].setAmbient(glm::vec3(0.1f, 0.1f, 0.1f));

    (*maxBlinnLights)[1].setPosition(glm::vec4(-5.0f, 3.0f, 5.0f, 1.0f));
    (*maxBlinnLights)[1].setEmittance(glm::vec3(30.0f, 30.0f, 30.0f));
    (*maxBlinnLights)[1].setAmbient(glm::vec3(0.1f, 0.1f, 0.1f));

    (*maxBlinnLights)[2].setPosition(glm::vec4(-10.0f, 3.0f, 12.0f, 1.0f));
    (*maxBlinnLights)[2].setEmittance(glm::vec3(10.0f, 10.0f, 10.0f));
    (*maxBlinnLights)[2].setAmbient(glm::vec3(0.05f, 0.05f, 0.05f));

    pbrLights = new LightArray(3, {(*defaultBatch)["pbr"]});
    (*pbrLights)[0].setPosition(glm::vec4(5.0f, 3.0f, -10.0f, 1.0f));
    (*pbrLights)[0].setEmittance(glm::vec3(300.0f, 300.0f, 300.0f));

    (*pbrLights)[1].setPosition(glm::vec4(-5.0f, 3.0f, 5.0f, 1.0f));
    (*pbrLights)[1].setEmittance(glm::vec3(300.0f, 300.0f, 300.0f));

    (*pbrLights)[2].setPosition(glm::vec4(-10.0f, 3.0f, 12.0f, 1.0f));
    (*pbrLights)[2].setEmittance(glm::vec3(100.0f, 100.0f, 100.0f));
}

void Builder3D::buildUniforms() {
    //Precomputing the required textures for IBL
    precompute();
    //Binding the appropriate textures for IBL
    miscellaneous.linkPrograms((*defaultBatch)["pbr"]);
    miscellaneous.linkUniform("exposure", &exposure);
    miscellaneous.linkUniform("LoD", &LoD - 1);
    miscellaneous.linkUniform("LuT", (*integratedFBO)[0]);
    miscellaneous.linkUniform("irradianceMap", (*convolutedFBOCube)[0]);
    miscellaneous.linkUniform("prefilterMap", (*prefilterFBOCube)[0]);

    miscellaneous.linkPrograms((*defaultBatch)["background"]);
    miscellaneous.linkUniform("exposure", &exposure);

    backgroundMaterial->linkUniform("envTexture", (*framebufferCube)[0]);
    postProcMaterial->linkUniform("rawTexture", [this] { return (*postProcFramebuffer)[0]; });
}

void Builder3D::draw(float dt, std::set<unsigned int> keysPressed) {
    defaultFramebuffer->resize(WindowSize::width, WindowSize::height);
    postProcFramebuffer->resize(WindowSize::width, WindowSize::height);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glClearDepth(1.0f);
    glClearColor(0.3f, 0.0f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (keysPressed.contains('c') && timeSinceLastHotreload >= 1.0f) {
        hotreload->unsubscribe(models[2]);
        hotreload = hotreload == (*defaultBatch)["maxblinn"] ? (*defaultBatch)["pbr"] : (*defaultBatch)["maxblinn"];
        models[2]->relink({hotreload});
        timeSinceLastHotreload = 0.0f;
    }

    camera->move(dt, keysPressed);

    postProcFramebuffer->bindBuffer(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    defaultBatch->executeAll();

    defaultFramebuffer->bindBuffer(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    postProcBatch->executeAll();

    timeSinceLastHotreload += dt;
}