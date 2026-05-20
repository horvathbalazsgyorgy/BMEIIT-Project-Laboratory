#include "builder3d.h"

#include "../quadmesh.h"
#include "../gmaterial.h"
#include "../../loader/assimpmodel.h"
#include "../../hdr/hdrtexture.h"
#include "../../hdr/hdrcube.h"

std::vector<std::pair<glm::vec3, glm::vec3>> calculatePoses(const float radius, const float count) {
    std::vector<std::pair<glm::vec3, glm::vec3>> poses;
    auto origo = glm::vec3(0.0f);
    auto limit = 2.0f * glm::pi<float>();
    auto x = 0.0f;
    while (x < limit) {
        auto rotation = glm::vec3(0.0f, glm::degrees(x), 0.0f);
        auto forward = glm::vec3(sin(x), 0.0f, cos(x));
        poses.emplace_back(origo + radius * -forward, rotation);
        x += limit/count;
    }
    return poses;
}

void Builder3D::setupSSBO() { //NOLINT
    //TODO: Some type of data structure for SSBOs so they don't just kind of exist
    glGenBuffers(1, &ssbo1);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo1);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * 9 * 32 + sizeof(float) * 32, nullptr, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo1);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &ssbo2);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo2);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * 9, nullptr, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo2);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Builder3D::precompute() { //NOLINT
    setupSSBO();
    //Initializing the required shaders
    auto precomputeBatch = std::make_unique<ShaderBatch>(std::vector<ShaderProperty>{
        {"computeConvolution", "convolution-sh-cs.glsl"},
        {"computeSum",         "sum-sh-cs.glsl"},
        {"equirectangular",    "envmapped-vs.glsl", "equirectangular-fs.glsl"},
        {"convolution",        "envmapped-vs.glsl", "convolution-fs.glsl"},
        {"prefilter",          "envmapped-vs.glsl", "prefilter-fs.glsl"},
        {"brdf",               "quad-vs.glsl",      "convolution-brdf-fs.glsl"}
    });

    auto cubeCamera = std::make_unique<Camera>(std::vector{
        (*precomputeBatch)["equirectangular"],
        (*precomputeBatch)["convolution"],
        (*precomputeBatch)["prefilter"]},
        glm::vec3(0.0f)
    );

    //Converting the equirectangular maps to cubemaps
    {
        framebufferCube->bindTarget(1, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, BILINEAR);
        auto material = std::make_unique<Material>((*precomputeBatch)["equirectangular"]);
        auto quad     = std::make_unique<QuadMesh>((*precomputeBatch)["equirectangular"], material.get());
        auto hdrCube  = std::make_unique<HDRCube>((*precomputeBatch)["equirectangular"], quad.get(), cubeCamera.get());
        //material->linkUniform("envTexture", envTexture);
        material->linkUniform("hdrTexture", hdrTexture);
        for (int i = 0; i < 6; i++) {
            framebufferCube->bindBuffer(i);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            precomputeBatch->executeOne("equirectangular");
        }
    }

    //Experimental - Generating the irradiance map using Spherical Harmonics
    {
        miscellaneous.linkPrograms((*precomputeBatch)["computeConvolution"]);
        miscellaneous.linkUniform("envTexture", (*framebufferCube)[0]);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo1);
        (*precomputeBatch)["computeConvolution"]->dispatch(glm::uvec3(8, 4, 1), GL_SHADER_STORAGE_BARRIER_BIT);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo2);
        (*precomputeBatch)["computeSum"]->dispatch(glm::uvec3(1, 1, 1), GL_SHADER_STORAGE_BARRIER_BIT);

        miscellaneous.flush();
    }

    (*framebufferCube)[0]->makeMipmap();

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
        prefilterFBOCube->bindTarget(1, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, TRILINEAR);
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
    gBufferBatch = new ShaderBatch({
        {"gbuffer",  "skinning-vs.glsl",  "gbuffer-fs.glsl"}  //G-Buffer program
    });

    defaultBatch = new ShaderBatch({
        {"maxblinn",  "quad-vs.glsl", "maxblinn-fs.glsl"},      //Max-Blinn program
        {"pbr",       "quad-vs.glsl", "cook-torrance-fs.glsl"}, //PBR program
    });

    backgroundBatch = new ShaderBatch({
        {"background", "envmapped-vs.glsl", "envmapped-fs.glsl"} //Background program
    });

    postProcBatch = new ShaderBatch({
        {"postproc", "quad-vs.glsl", "postproc-fs.glsl"}, //Post-proc program
    });

    hotreload = (*defaultBatch)["pbr"];
}

void Builder3D::buildMeshes() {
    backgroundMesh = new QuadMesh((*backgroundBatch)["background"], backgroundMaterial);
    postProcMesh = new QuadMesh((*postProcBatch)["postproc"], postProcMaterial);
    gBufferMesh  = new QuadMesh(hotreload, gBufferMaterial);
}

void Builder3D::buildMaterials() {
    /*
    envTexture = new TextureCube(LINEAR, {
    "background/ldr/nowhere/posx.jpg",
    "background/ldr/nowhere/negx.jpg",
    "background/ldr/nowhere/posy.jpg",
    "background/ldr/nowhere/negy.jpg",
    "background/ldr/nowhere/posz.jpg",
    "background/ldr/nowhere/negz.jpg"});
    */

    //envTexture = new HDRTexture("background/hdr/outdoor/shanghai_bund_8k.hdr");
    hdrTexture = new HDRTexture("background/hdr/outdoor/shanghai_bund_8k.hdr");
    backgroundMaterial = new Material((*backgroundBatch)["background"]);
    postProcMaterial = new Material((*postProcBatch)["postproc"]);
    gBufferMaterial  = new GMaterial(hotreload, gBuffer);
}

void Builder3D::buildModels() {
    models.push_back(new Model(hotreload, {gBufferMesh}));
    models.push_back(new Model((*postProcBatch)["postproc"], {postProcMesh}));
    models.push_back(new Model((*backgroundBatch)["background"], {backgroundMesh}));

    auto poses = calculatePoses(10.0f, 4);
    models.push_back(new AssimpModel((*gBufferBatch)["gbuffer"],
        "assimp/showroom/forgotten_knight/scene.gltf",
        poses[0].first,
          glm::vec3(3.0f, 3.0f, 3.0f),
        poses[0].second,
        ALBEDO, NORMAL, AMBIENT_OCCLUSION, METALLIC_ROUGHNESS));

    models.push_back(new AssimpModel((*gBufferBatch)["gbuffer"],
        "assimp/showroom/sir_frog/scene.gltf",
        poses[1].first,
          glm::vec3(4.0f, 4.0f, 4.0f),
        poses[1].second,
    ALBEDO, NORMAL, AMBIENT_OCCLUSION, METALLIC_ROUGHNESS));

    models.push_back(new AssimpModel((*gBufferBatch)["gbuffer"],
        "assimp/showroom/silksong_hornet/scene.gltf",
        poses[2].first,
          glm::vec3(5.0f, 5.0f, 5.0f),
        poses[2].second,
    ALBEDO, NORMAL, AMBIENT_OCCLUSION, METALLIC_ROUGHNESS));

    models.push_back(new AssimpModel((*gBufferBatch)["gbuffer"],
        "assimp/showroom/the_phantom_rogue/scene.gltf",
        glm::vec3(poses[3].first.x, 10.0f, poses[3].first.z),
          glm::vec3(4.0f, 4.0f, 4.0f),
        poses[3].second,
    ALBEDO, NORMAL, AMBIENT_OCCLUSION, METALLIC_ROUGHNESS));
}

void Builder3D::buildFramebuffers() {
    defaultFramebuffer  = new DefaultFramebuffer();
    framebufferCube     = new FramebufferCube(1024, 1024, 1);
    convolutedFBOCube   = new FramebufferCube(64, 64, 1);
    prefilterFBOCube    = new FramebufferCube(128, 128, (int)LoD + 1);
    integratedFBO       = new Framebuffer(512, 512, 1);
    postProcFramebuffer = new Framebuffer(WindowSize::width, WindowSize::height, 1);
    postProcFramebuffer->bindTarget(1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, POINT);

    /**
     * G-BUFFER
     **/
    gBuffer = new Framebuffer(WindowSize::width, WindowSize::height, 1);
    /**
     * 1. Position
     * 2. Normal
     * 3. Albedo
     * 4. Ambient Occlusion, Roughness, Metallic
     **/
    gBuffer->bindTarget(4, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, POINT);
}

void Builder3D::buildCamera() {
    camera = new Camera(
        {(*gBufferBatch)["gbuffer"],
                    (*defaultBatch)["maxblinn"],
                    (*defaultBatch)["pbr"],
                    (*backgroundBatch)["background"]},
        glm::vec3(-11.0f, 4.5f, -16.5f),
        -0.5f, 57.0f
    );
    camera->setSpeed(5.0f);
}

void Builder3D::buildLights() {
    maxBlinnLights = new LightArray(4, {(*defaultBatch)["maxblinn"]});
    (*maxBlinnLights)[0].setPosition(glm::vec4(-15.0f, 25.0f, -15.0f, 1.0f));
    (*maxBlinnLights)[0].setEmittance(glm::vec3(200.0f, 190.0f, 160.0f));
    (*maxBlinnLights)[0].setAmbient(glm::vec3(0.1f, 0.095f, 0.08f));

    (*maxBlinnLights)[1].setPosition(glm::vec4(-5.0f, 15.0f, -5.0f, 1.0f));
    (*maxBlinnLights)[1].setEmittance(glm::vec3(100.0f, 95.0f, 80.0f));
    (*maxBlinnLights)[1].setAmbient(glm::vec3(0.05f, 0.047f, 0.04f));

    (*maxBlinnLights)[2].setPosition(glm::vec4(5.0f, 15.0f, 5.0f, 1.0f));
    (*maxBlinnLights)[2].setEmittance(glm::vec3(100.0f, 95.0f, 80.0f));
    (*maxBlinnLights)[2].setAmbient(glm::vec3(0.05f, 0.047f, 0.04f));

    (*maxBlinnLights)[3].setPosition(glm::vec4(15.0f, 25.0f, 15.0f, 1.0f));
    (*maxBlinnLights)[3].setEmittance(glm::vec3(200.0f, 190.0f, 160.0f));
    (*maxBlinnLights)[3].setAmbient(glm::vec3(0.1f, 0.095f, 0.08f));

    pbrLights = new LightArray(4, {(*defaultBatch)["pbr"]});
    (*pbrLights)[0].setPosition(glm::vec4(-15.0f, 25.0f, -15.0f, 1.0f));
    (*pbrLights)[0].setEmittance(glm::vec3(750.0f, 712.0f, 600.0f));

    (*pbrLights)[1].setPosition(glm::vec4(-5.0f, 15.0f, -5.0f, 1.0f));
    (*pbrLights)[1].setEmittance(glm::vec3(500.0f, 475.0f, 400.0f));

    (*pbrLights)[2].setPosition(glm::vec4(5.0f, 15.0f, 5.0f, 1.0f));
    (*pbrLights)[2].setEmittance(glm::vec3(500.0f, 475.0f, 400.0f));

    (*pbrLights)[3].setPosition(glm::vec4(15.0f, 25.0f, 15.0f, 1.0f));
    (*pbrLights)[3].setEmittance(glm::vec3(750.0f, 712.0f, 600.0f));
}

void Builder3D::buildUniforms() {
    //Precomputing the required textures for IBL
    precompute();
    //Binding the appropriate textures for IBL
    miscellaneous.linkPrograms((*defaultBatch)["pbr"]);
    miscellaneous.linkUniform("spherical", &spherical);
    miscellaneous.linkUniform("exposure", &exposure);
    miscellaneous.linkUniform("LoD", &LoD);
    miscellaneous.linkUniform("LuT", (*integratedFBO)[0]);
    miscellaneous.linkUniform("irradianceMap", (*convolutedFBOCube)[0]);
    miscellaneous.linkUniform("prefilterMap", (*prefilterFBOCube)[0]);

    backgroundMaterial->linkUniform("exposure", &exposure);
    backgroundMaterial->linkUniform("envTexture", (*framebufferCube)[0]);

    postProcMaterial->linkUniform("rawTexture", [this] { return (*postProcFramebuffer)[0]; });
}

void Builder3D::draw(float dt, std::set<unsigned int> keysPressed) {
    defaultFramebuffer->resize(WindowSize::width, WindowSize::height);
    postProcFramebuffer->resize(WindowSize::width, WindowSize::height);
    gBuffer->resize(WindowSize::width, WindowSize::height);

    ThreadPool::processCompletedJobs();

    glDepthFunc(GL_LESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (keysPressed.contains('c') && timeSinceLastHotreload >= 1.0f) {
        auto newReload = hotreload == (*defaultBatch)["maxblinn"] ? (*defaultBatch)["pbr"] : (*defaultBatch)["maxblinn"];
        hotreload->unsubscribe(models[0]);
        models[0]->relink({newReload});
        hotreload = newReload;
        timeSinceLastHotreload = 0.0f;
    }

    if (keysPressed.contains('i') && timeSinceLastIrradianceChange >= 1.0f) {
        spherical = !spherical;
        timeSinceLastIrradianceChange = 0.0f;
    }

    camera->move(dt, keysPressed);

    /* ===================================================== *
     * GEOMETRY PASS
     * ===================================================== */
    gBuffer->bindBuffer(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    gBufferBatch->executeAll();

    /* ===================================================== *
     * LIGHTING PASS
     * ===================================================== */
    postProcFramebuffer->bindBuffer(0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    gBuffer->syncDepth(postProcFramebuffer);
    postProcFramebuffer->bindBuffer(0);
    defaultBatch->executeAll();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    backgroundBatch->executeAll();

    /* ===================================================== *
     * POST-PROCESS PASS
     * ===================================================== */
    defaultFramebuffer->bindBuffer(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    postProcBatch->executeAll();

    timeSinceLastHotreload += dt;
    timeSinceLastIrradianceChange += dt;
}